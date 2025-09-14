/**
 ******************************************************************************
 * @file    bsp_screen.c
 * @author  RandleH
 * @brief   Board Support Package Delivery - Screen
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 RandleH.
 * All rights reserved.
 *
 * This software component is licensed by RandleH under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
*/



/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include <stdbool.h>
#include "device.h"
#include "global.h"
#include "bsp_screen.h"
#include "bsp_timer.h"
#include "cmn_delay.h"


/* ************************************************************************** */
/*                               Private Macros                               */
/* ************************************************************************** */
#define CMD    0
#define DAT    1

#define PIN_DC(x)\
  do{\
    if((x)==0){\
      ((SCREEN_DC_GPIO_Port)->BSRR) |= (u32)((SCREEN_DC_Pin)<<16);\
    }else{\
      ((SCREEN_DC_GPIO_Port)->BSRR) |= (SCREEN_DC_Pin);\
    }\
  }while(0)

#if BSP_SCREEN_USE_HARDWARE_NSS
  #define PIN_CS(x)
#else
  #define PIN_CS(x)\
    do{\
      if((x)==0){\
        ((SCREEN_CS_GPIO_Port)->BSRR) |= (u32)((SCREEN_CS_Pin)<<16);\
      }else{\
        ((SCREEN_CS_GPIO_Port)->BSRR) |= (SCREEN_CS_Pin);\
      }\
    }while(0)
#endif

static tBspScreen *THIS = &metope.bsp.screen;

/* ************************************************************************** */
/*                             Private Functions                              */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief BSP Screen Block SPI transmission function
 * @param [in] buf    - Data Buffer
 * @param [in] nItems - Data Length
 * @param [in] nTimes - How many times do you want to transmit?
 * @addtogroup MachineDependent
 */
STATIC cmnBoolean_t bsp_screen_spi_polling_send( const uint8_t *buf, size_t nItems, size_t nTimes){
  if( buf==NULL || nItems==0){
    return 1;
  }

  while(nTimes--){
#if 1
    CLEAR_BIT( SPI2->CR1, SPI_CR1_DFF);
    SET_BIT( SPI2->CR1, SPI_CR1_BIDIOE);
    if( !READ_BIT( SPI2->CR1, SPI_CR1_SPE) ){
      SET_BIT( SPI2->CR1, SPI_CR1_SPE);
    }
    const uint8_t *ptr = buf;
    size_t len = nItems;
    while( len--){
      SPI2->DR = *ptr++;
      while( 0==READ_BIT( SPI2->SR, SPI_SR_TXE));  // Blocking function
    }
#else
    HAL_SPI_Transmit( &hspi2, buf, nItems, HAL_MAX_DELAY);
    while( hspi2.State == HAL_SPI_STATE_BUSY );
#endif
  }
  
  return 0;
}

/**
 * @brief BSP Screen Non-Block SPI transmission function using DMA
 * @param [in] buf    - Data Buffer
 * @param [in] nItems - Data Length
 * @param [in] nTimes - How many times do you want to transmit?
 * @addtogroup MachineDependent
 */
STATIC cmnBoolean_t bsp_screen_spi_dma_send( const uint8_t *buf, size_t nItems, size_t nTimes){
  if( buf==NULL || nItems==0){
    return 1;
  }
  
  cmnBoolean_t ret = SUCCESS;

  while(nTimes--){
    metope.dev.status.spi2 = BUSY;

#if 1
    metope.dev.pHspi2->State       = HAL_SPI_STATE_BUSY_TX;
    metope.dev.pHspi2->ErrorCode   = HAL_SPI_ERROR_NONE;
    /* Clear DBM bit */
    DMA1_Stream4->CR &= (uint32_t)(~DMA_SxCR_DBM);

  /* Configure DMA Stream data length */
    DMA1_Stream4->NDTR = nItems;

    /* Configure DMA Stream destination address */
    DMA1_Stream4->PAR = ((u32)(&(SPI2->DR)));

    /* Configure DMA Stream source address */
    DMA1_Stream4->M0AR = (u32)buf;

    DMA1->HIFCR = (0x3FU << (4-4)) ;

    /* Enable Common interrupts*/
    DMA1_Stream4->CR |= DMA_IT_TC | DMA_IT_TE | DMA_IT_DME;

    /* Enable Half Complete Interrupt */
    DMA1_Stream4->CR |= DMA_IT_HT;
    
    /* Enable the Peripheral */
    DMA1_Stream4->CR |= DMA_SxCR_EN;

    /* Check if the SPI is already enabled */
    if ((SPI2->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE){
      /* Enable SPI peripheral */
      SET_BIT( SPI2->CR1, SPI_CR1_SPE);
    }

#ifdef DEBUG
    /* Enable Error Interrupt */
    SET_BIT(SPI2->CR2, SPI_IT_ERR);
#endif

    SET_BIT(SPI2->CR2, SPI_CR2_TXDMAEN);
#else
    if(HAL_OK!=HAL_SPI_Transmit_DMA( metope.dev.pHspi2, (u8*)buf, nItems)){
      ret = ERROR;
      break;
    }
#endif
    
    /**
     * @note
     *  Phase 1: DMA Request to send bytes
     *  Phase 2: DMA Interrupt will be triggered. Entering the `DMA1_Stream4_IRQHandler()`
     *  Phase 3: `HAL_DMA_IRQHandler()` will be called.
     *  Phase 4: `SPI_DMATransmitCplt()` function will be called. This callback was configed during DMA requesting to send.
     *  Phase 5: `HAL_SPI_TxCpltCallback()` will be called if no error. This is a weak function.
     *  Phase 6: The escape function shall be notified to come back.
     */
    if(metope.app.rtos.status.running){
      xEventGroupWaitBits( metope.app.rtos.event._handle, CMN_EVENT_SCREEN_REFRESH_CPLT, pdTRUE, pdFALSE, portMAX_DELAY);
    }else{
      while(IDLE!=metope.dev.status.spi2);
    }
  }
  return ret;
}

/**
 * @brief Internal function. Given a predefined transmission code and transfer it in blocking mode. DMA bypass.
 * @param [in] code - Code buffer
 * @param [in] len  - Buffer length
*/
STATIC void bsp_screen_parse_code( const uint8_t *code, size_t len){
  while( len!=0 ){
    PIN_DC( *code++);       /* Determine command or data */
    --len;
    uint8_t nItem = *code++;     /* Parse num of items */
    --len;

    bsp_screen_spi_polling_send( code, nItem, 1);
    code += nItem;
    len  -= nItem;
  }
}


/**
 * @brief Internal function. Select screen area. DMA bypass.
 * @param [in] x0 - Start X
 * @param [in] y0 - Start Y
 * @param [in] x1 - End X
 * @param [in] y1 - End Y
*/
STATIC void bsp_screen_area( u8 x0, u8 y0, u8 x1, u8 y1){
    const u8 code[] = {
        /* Set the X coordinates */
        CMD, 1, 0x2A,\
        DAT, 4, 0x00,  x0,0x00,  x1,\
        
        /* Set the Y coordinates */
        CMD, 1, 0x2B,\
        DAT, 4, 0x00,  y0,0x00,  y1,\

        CMD, 1, 0x2C
    };
    bsp_screen_parse_code( code, sizeof(code)/sizeof(*code));
}

/**
 * @brief Change scan direction. DMA bypass.
 * @param [in] mode - 0: L->R; U->D; 1: R->L; D->U;
*/
STATIC void bsp_screen_scan_mode( u8 mode){
  u8 code[] = {
    CMD, 1, 0x36,\
    DAT, 1, 0x08
  };

  switch(mode){
    case 0: // L->R; U->D;
      code[5] = 0x08;
      break;
    case 1: // R->L; D->U;
      code[5] = 0xC8;
      break;
    default:
      break;
  }

  bsp_screen_parse_code( code, sizeof(code)/sizeof(*code));
}

#ifdef __cplusplus
}
#endif

/* ************************************************************************** */
/*                              Public Functions                              */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief
 * @param [in]  value - 0~2047
 * @addtogroup MachineDependent
 */
void inline bsp_screen_set_bright( bspScreenBrightness_t value){
  /**
   * @note
   *                    (f_apb MHz) * 1000000
   * f_tclk = ------------------------------------------ Hz = 2048 KHz
   *                  (f_apb MHz) * 1000 / 2048
   *                 |-------------+-----------|
   *                               |
   *                               +------------------------->   prescaler value; should between 1 ~ 65535
   * 
   * By default, we make timer input clk source divided to 2.048 MHz which is easier for later use
   * 
  */
#if 1
  TIM3->CCR1 = value;
#else
  __HAL_TIM_SET_COMPARE( &htim3, TIM_CHANNEL_1, value);
#endif
}

/**
 * @brief
 * @param [in] delta  - 0:255
 * @param [in] cw_ccw - cw=0 ccw=1
 */
void bsp_screen_rotate( bspScreenRotate_t delta, uint8_t cw_ccw) {
  const u8 parameter[] = {0b00001000, 0b01101000, 0b10001000, 0b10101000}; // cw => | ccw <= 

  u8 rotation = THIS->rotation;
  
  delta    %= 4;
  rotation += 4;
  rotation  = (cw_ccw != 0) ? ((rotation - delta) % 4) : ((rotation + delta) % 4);

  const u8 code[] = {
    /* Set the MAC */
    CMD, 1, 0x36 ,\
    DAT, 1, parameter[rotation]
  };
  bsp_screen_parse_code( code, sizeof(code)/sizeof(*code));

  THIS->rotation = rotation;
}

/**
 * @brief
 * @addtogroup MachineDependent
 */
void inline bsp_screen_on(void){
#if 1
  htim3.ChannelState[TIM_CHANNEL_1] = HAL_TIM_CHANNEL_STATE_BUSY;
  TIM3->CCER &= ~(TIM_CCER_CC1E << (TIM_CHANNEL_1 & 0x1FU));
  TIM3->CCER |= 1<<(TIM_CHANNEL_1& 0x1FU);
  TIM3->CR1  |= TIM_CR1_CEN;
#else
  HAL_TIM_PWM_Start( &htim3, TIM_CHANNEL_1);
#endif
}

/**
 * @brief
 * @addtogroup MachineDependent
 */
void inline bsp_screen_off(void){
  bsp_screen_set_bright(0);
}


void bsp_screen_smooth_off(void){
  u32 brightness = THIS->brightness;
  do{
    brightness >>= 1;
    bsp_screen_set_bright(brightness);
    cmn_tim9_sleep(20000, metope.app.rtos.status.running==true); // 20ms
  }while(brightness);
}

void bsp_screen_smooth_on(void){
  u32 brightness = 1;
  while(brightness < THIS->brightness){
    brightness <<= 1;
    bsp_screen_set_bright(brightness);
    cmn_tim9_sleep(20000, metope.app.rtos.status.running==true); // 20ms
  }
  bsp_screen_set_bright(THIS->brightness);
}


/**
 * @brief Bsp Screen Initialization
 * @return `SUCCESS` | `ERROR`
 * @attention
 *  `bsp_timer_init()` must be called at first.
 */
cmnBoolean_t bsp_screen_init( void){
  bsp_screen_set_bright(0);
  bsp_screen_on();
  PIN_CS(0);
  const uint8_t init_code[] = {
        CMD,  1, 0x36,\
        DAT,  1, 0xc8,\
        CMD,  2, 0xef,0xeb,\
        DAT,  1, 0x14,\
        CMD,  3, 0xfe,0xef,0xeb,\
        DAT,  1, 0x14,\
        CMD,  1, 0x84,\
        DAT,  1, 0x40,\
        CMD,  1, 0x85,\
        DAT,  1, 0xff,\
        CMD,  1, 0x86,\
        DAT,  1, 0xff,\
        CMD,  1, 0x87,\
        DAT,  1, 0xff,\
        CMD,  1, 0x88,\
        DAT,  1, 0x0a,\
        CMD,  1, 0x89,\
        DAT,  1, 0x21,\
        CMD,  1, 0x8a,\
        DAT,  1, 0x00,\
        CMD,  1, 0x8b,\
        DAT,  1, 0x80,\
        CMD,  1, 0x8c,\
        DAT,  1, 0x01,\
        CMD,  1, 0x8d,\
        DAT,  1, 0x01,\
        CMD,  1, 0x8e,\
        DAT,  1, 0xff,\
        CMD,  1, 0x8f,\
        DAT,  1, 0xff,\
        CMD,  1, 0xb6,\
        DAT,  2, 0x00,0x20,\
        CMD,  1, 0x36,\
        DAT,  1, 0xc8,\
        CMD,  1, 0x3a,\
        DAT,  1, 0x05,\
        CMD,  1, 0x90,\
        DAT,  4, 0x08,0x08,0x08,0x08,\
        CMD,  1, 0xbd,\
        DAT,  1, 0x06,\
        CMD,  1, 0xbc,\
        DAT,  1, 0x00,\
        CMD,  1, 0xff,\
        DAT,  3, 0x60,0x01,0x04,\
        CMD,  1, 0xc3,\
        DAT,  1, 0x13,\
        CMD,  1, 0xc4,\
        DAT,  1, 0x13,\
        CMD,  1, 0xc9,\
        DAT,  1, 0x22,\
        CMD,  1, 0xbe,\
        DAT,  1, 0x11,\
        CMD,  1, 0xe1,\
        DAT,  2, 0x10,0x0e,\
        CMD,  1, 0xdf,\
        DAT,  3, 0x21,0x0c,0x02,\
        CMD,  1, 0xf0,\
        DAT,  6, 0x45,0x09,0x08,0x08,0x26,0x2a,\
        CMD,  1, 0xf1,\
        DAT,  6, 0x43,0x70,0x72,0x36,0x37,0x6f,\
        CMD,  1, 0xf2,\
        DAT,  6, 0x45,0x09,0x08,0x08,0x26,0x2a,\
        CMD,  1, 0xf3,\
        DAT,  6, 0x43,0x70,0x72,0x36,0x37,0x6f,\
        CMD,  1, 0xed,\
        DAT,  2, 0x1b,0x0b,\
        CMD,  1, 0xae,\
        DAT,  1, 0x77,\
        CMD,  1, 0xcd,\
        DAT,  1, 0x63,\
        CMD,  1, 0x70,\
        DAT,  9, 0x07,0x07,0x04,0x0e,0x0f,0x09,0x07,0x08,0x03,\
        CMD,  1, 0xe8,\
        DAT,  1, 0x34,\
        CMD,  1, 0x62,\
        DAT, 12, 0x18,0x0d,0x71,0xed,0x70,0x70,0x18,0x0f,0x71,0xef,0x70,0x70,\
        CMD,  1, 0x63,\
        DAT, 12, 0x18,0x11,0x71,0xf1,0x70,0x70,0x18,0x13,0x71,0xf3,0x70,0x70,\
        CMD,  1, 0x64,\
        DAT,  7, 0x28,0x29,0xf1,0x01,0xf1,0x00,0x07,\
        CMD,  1, 0x66,\
        DAT, 10, 0x3c,0x00,0xcd,0x67,0x45,0x45,0x10,0x00,0x00,0x00,\
        CMD,  1, 0x67,\
        DAT, 10, 0x00,0x3c,0x00,0x00,0x00,0x01,0x54,0x10,0x32,0x98,\
        CMD,  1, 0x74,\
        DAT,  7, 0x10,0x85,0x80,0x00,0x00,0x4e,0x00,\
        CMD,  1, 0x98,\
        DAT,  2, 0x3e,0x07,\
        CMD,  3, 0x35,0x21,0x11
    };
  bsp_screen_parse_code( init_code, sizeof(init_code)/sizeof(uint8_t));

  /**
   * @attention
   *  `bsp_timer_init()` must be called at first.
   */
  cmnBoolean_t async_mode = false;
  cmn_tim2_sleep( 120, async_mode);

  const u8 on_code[] = {
      CMD, 1, 0x29,\
  };
  bsp_screen_parse_code( on_code, sizeof(on_code)/sizeof(uint8_t));

  cmn_tim2_sleep( 20, async_mode);
  
  bsp_screen_scan_mode(0);
  PIN_CS(1);

  return SUCCESS;
}


/**
 * @brief Fill screen with single color
 * @attention Length of the buffer must be valid great or equal than (xe-xs+1)*(ye-ys+1)
 * @param [in] color - Color aligned with the screen color depth
 * @param [in] xs    - Coordinates
 * @param [in] ys    - Coordinates
 * @param [in] xe    - Coordinates
 * @param [in] ye    - Coordinates
*/
void bsp_screen_fill( const bspScreenPixel_t color, bspScreenCood_t xs, bspScreenCood_t ys, bspScreenCood_t xe, bspScreenCood_t ye){
  if( xs>xe || ys>ye ){
      return;
  }
  
  u8 buf[2] = {(u8)(color>>8),(u8)(color&0xff)};
  
  PIN_CS(0);
  bsp_screen_area( xs, ys, xe, ye);
  
  PIN_DC(1);
  bsp_screen_spi_polling_send( buf, sizeof(color), (xe-xs+1)*(ye-ys+1));
  PIN_CS(1);
}

/**
 * @brief Refresh screen within an area
 * @param [in] buf - Color buffer aligned with the screen color depth
 * @param [in] xs  - Coordinates
 * @param [in] ys  - Coordinates
 * @param [in] xe  - Coordinates
 * @param [in] ye  - Coordinates
 */
void bsp_screen_refresh( const bspScreenPixel_t *buf, bspScreenCood_t xs, bspScreenCood_t ys, bspScreenCood_t xe, bspScreenCood_t ye){
  PIN_CS(0);
  bsp_screen_area( xs, ys, xe, ye);
  PIN_DC(1);
  bsp_screen_spi_polling_send( (const u8*)buf, (xe-xs+1)*(ye-ys+1)*sizeof(bspScreenPixel_t), 1);
  PIN_CS(1);
}


/**
 * @brief Screen Circular Refresh Function
 * @note  Recommanded stack depth: 512 Bytes
 * @param [in] param  - will be cast to `tBspScreen *`
 */
void bsp_screen_main(void *param) RTOSTHREAD{
  tBspScreen *parsed_param = (tBspScreen *)(param);
  TickType_t  xLastWakeTime = xTaskGetTickCount();
  while(1){
    vTaskDelayUntil( &xLastWakeTime, parsed_param->refresh_rate_ms);
    lv_tick_inc(parsed_param->refresh_rate_ms);
    lv_timer_handler();
  }
}

/**
 * @brief Screen ON/OFF switch
 * @note  Recommanded stack depth: 128 Bytes
 * @param [in] param  - NOT used
 */
void bsp_screen_onoff(void *param) RTOSTHREAD{
  UNUSED(param);
  
  metope.info.status.scroff = false;
  bsp_screen_smooth_on();
  xEventGroupClearBits( metope.app.rtos.event._handle, CMN_EVENT_USER_KEY_M);
  
  while(1){
    EventBits_t uxBits = xEventGroupWaitBits( metope.app.rtos.event._handle, 
      CMN_EVENT_USER_KEY_M    |
      CMN_EVENT_SCREEN_DISPOFF|
      CMN_EVENT_SCREEN_DISPON |
      CMN_EVENT_SCREEN_DISPBR, pdTRUE, pdFALSE, portMAX_DELAY);
    
    /* Always process the user button clicking first */
    if(uxBits & CMN_EVENT_USER_KEY_M){
      if(metope.info.status.scroff==true){
        bsp_screen_smooth_on();
      }else{
        bsp_screen_smooth_off();
      }
      metope.info.status.scroff = !metope.info.status.scroff;
    }
    /* Now it's time to process the long time inactive screen */
    else if(uxBits & CMN_EVENT_SCREEN_DISPOFF) {
      bsp_screen_smooth_off();
      metope.info.status.scroff = 1;
    }
    else if(uxBits & CMN_EVENT_SCREEN_DISPON) {
      bsp_screen_smooth_on();
      metope.info.status.scroff = 0;
    }
    
    if(uxBits & CMN_EVENT_SCREEN_DISPBR) {
      bsp_screen_set_bright(THIS->brightness);
    }
  }
}


#ifdef __cplusplus
}
#endif


/* ********************************** EOF *********************************** */

