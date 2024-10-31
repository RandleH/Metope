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




#include <stdbool.h>
#include "global.h"
#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  #include "stm32f4xx_hal.h"
  #include "stm32f4xx_hal_tim.h"
#endif
#include "bsp_screen.h"
#include "cmn_delay.h"

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

#define PIN_CS(x)
#if 0
#define PIN_CS(x)\
  do{\
    if((x)==0){\
      ((SCREEN_CS_GPIO_Port)->BSRR) |= (u32)((SCREEN_CS_Pin)<<16);\
    }else{\
      ((SCREEN_CS_GPIO_Port)->BSRR) |= (SCREEN_CS_Pin);\
    }\
  }while(0)
#endif

#ifdef __cplusplus
extern "C"{
#endif


/**
 * @brief
 * @addtogroup MachineDependent
 */
cmnBoolean_t bsp_screen_block_send( const uint8_t *buf, size_t nItems, size_t nTimes, uint32_t interval_delay_ms, uint8_t* pDone){
  if( pDone!=NULL){
    *pDone = false;
  }

  if( buf==NULL || nItems==0){
    return 1;
  }
  
  // CLEAR_BIT( SPI2->CR1, SPI_CR1_DFF);
  // SET_BIT( SPI2->CR1, SPI_CR1_BIDIOE);
  // if( !READ_BIT( SPI2->CR1, SPI_CR1_SPE) ){
  //   SET_BIT( SPI2->CR1, SPI_CR1_SPE);
  // }
  
  while(nTimes--){
    // const uint8_t *ptr = buf;
    // size_t len = nItems;
    // while( len--){
    //   SPI2->DR = *ptr++;
    //   while( 0==READ_BIT( SPI2->SR, SPI_SR_TXE));  // Blocking function
    // }
    HAL_SPI_Transmit( &hspi2, buf, nItems, HAL_MAX_DELAY);
    while( hspi2.State == HAL_SPI_STATE_BUSY );

    // if( interval_delay_ms!=0){
      // rh_cmn_delay_ms__escape( interval_delay_ms);
    // }
  }
  
  if( pDone!=NULL){
    *pDone = true;
  }
  
  return 0;
}

/**
 * @brief
 * @param [in]  value - 0~2047
 * @addtogroup MachineDependent
 */
void bsp_screen_set_bright( bspScreenBrightness_t value){
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
  __HAL_TIM_SET_COMPARE( &htim3, TIM_CHANNEL_1, value);
}

/**
 * @brief
 * @addtogroup MachineDependent
 */
void inline bsp_screen_on(void){
  HAL_TIM_PWM_Start( &htim3, TIM_CHANNEL_1);
}

/**
 * @brief
 * @addtogroup MachineDependent
 */
void inline bsp_screen_off(void){
  // HAL_TIM_PWM_Stop( &htim3, TIM_CHANNEL_1);
  bsp_screen_set_bright(0);
}

const u16 tmp[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377};

void bsp_screen_smooth_on(void){
  u32 cnt = 0;
  while(cnt < sizeof(tmp)/sizeof(*tmp)){
    bsp_screen_set_bright(tmp[cnt++]);
    cmn_timer_delay(50);
  }
}

void bsp_screen_smooth_off(void){
  u32 cnt = sizeof(tmp)/sizeof(*tmp);
  while(cnt--){
    bsp_screen_set_bright(tmp[cnt]);
    cmn_timer_delay(50);
  }
}


/**
 * @brief       Internal function. Given a predefined transmission code and transfer it in blocking mode. DMA bypass.
 * @param [in]  code  - Code buffer
 * @param [in]  len   - Buffer length
 * @return      (none)
*/
static void bsp_screen_parse_code( const uint8_t *code, size_t len){
  while( len!=0 ){
    PIN_DC( *code++);       /* Determine command or data */
    --len;
    uint8_t nItem = *code++;     /* Parse num of items */
    --len;

    cmnBoolean_t done = false;
    bsp_screen_block_send( code, nItem, 1, 0, &done);
    while(!done){
        //...//
    }
    code += nItem;
    len  -= nItem;
  }
}




/**
 * @brief       Internal function. Select screen area. DMA bypass.
 * @param       x0  Start X
 * @param       y0  Start Y
 * @param       x1  End X
 * @param       y1  End Y
 * @return      (none)
*/
static void bsp_screen_area( u8 x0, u8 y0, u8 x1, u8 y1){
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
 * @brief       Change scan direction. DMA bypass.
 * @param       mode    @ref 0: L->R; U->D;
 *                      @ref 1: R->L; D->U;
 * @return      (none)
*/
static void bsp_screen_scan_mode( u8 mode){
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


/**
 * @brief 
 */
cmnBoolean_t bsp_screen_init( void){
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

  cmn_timer_delay(120);

  const u8 on_code[] = {
      CMD, 1, 0x29,\
  };
  bsp_screen_parse_code( on_code, sizeof(on_code)/sizeof(uint8_t));

  cmn_timer_delay(20);
  bsp_screen_scan_mode(0);
  PIN_CS(1);
  return true;
}


/**
 * @brief       Fill screen with single color
 *              Length of the buffer must be valid great or equal than (xe-xs+1)*(ye-ys+1)
 * @return      Return 0, if success
 *              Return 1, if area params are wrong
*/
u32 bsp_screen_fill( const u16 color, u16 xs, u16 ys, u16 xe, u16 ye){
    if( xs>xe || ys>ye ){
        return 1;
    }
    
    u8 buf[2] = {(u8)(color>>8),(u8)(color&0xff)};
    u8 done = false;
    PIN_CS(0);
    bsp_screen_area( xs, ys, xe, ye);
    
    PIN_DC(1);
    bsp_screen_block_send( buf, sizeof(color), (xe-xs+1)*(ye-ys+1), 0, &done);
    
    if(!done){
        // #warning "Note: Do something"
    }
    
    PIN_CS(1);

    return 0;
}



#ifdef __cplusplus
}
#endif







