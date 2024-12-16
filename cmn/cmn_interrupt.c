/**
 ******************************************************************************
 * @file    cmn_interrupt.c
 * @author  RandleH
 * @brief   Interrupt Service Routine
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
#include "cmn_type.h"
#include "cmn_device.h"
#include "cmn_interrupt.h"
#include "cmn_utility.h"
#include "cmn_callback.h"
#include "bsp_led.h"
#include "global.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#ifdef __cplusplus
extern "C"{
#endif

/** @defgroup Common 
 *  Project Common Group
 *  @{
 */



/* ************************************************************************** */
/*                       Interrupt Management Macros                          */
/* ************************************************************************** */
#define THIS                        (&gInterruptProcess)

#define LEFT_CHILD_OF(x)            (((x)<<1)+1)
#define RIGHT_CHILD_OF(x)           (((x)<<1)+2)
#define PARENT_OF(x)                (((x)-1)>>1)
#define FIRST_PARENT(x)             (((x)-2)>>1)


/* ************************************************************************** */
/*                      Interrupt Management Variable                         */
/* ************************************************************************** */
tCmnInterruptProc gInterruptProcess = {0};
extern DMA_HandleTypeDef hdma_spi2_tx;
extern SPI_HandleTypeDef hspi2;



/* ************************************************************************** */
/*                       Interrupt Management Function                        */
/* ************************************************************************** */
STATIC cmnBoolean_t cmn_interrupt_push( const tCmnInterruptUnit *unit){
  if(THIS->record_cnt==NUM_PENDING_INTERRUPT){
    return false;
  }
  
  uint8_t cnt = THIS->record_cnt;
  THIS->record[THIS->record_cnt++] = (*unit);
  
  while(cnt){
    if( THIS->record[PARENT_OF(cnt)].target_cnt > THIS->record[cnt].target_cnt){
      CMN_SWAP( THIS->record[PARENT_OF(cnt)].operation, THIS->record[cnt].operation);
      CMN_SWAP( THIS->record[PARENT_OF(cnt)].irqn, THIS->record[cnt].irqn);
      CMN_SWAP( THIS->record[PARENT_OF(cnt)].target_cnt, THIS->record[cnt].target_cnt);
    }else{
      break;
    }
    cnt = PARENT_OF(cnt);
  }
  
  return true;
}


STATIC void cmn_interrupt_pop( void){
  if(THIS->record_cnt==0){
    return;
  }
  uint8_t cnt = 0;
  --THIS->record_cnt;

  while( cnt<THIS->record_cnt){
    uint8_t L_CHILD = LEFT_CHILD_OF(cnt);
    uint8_t R_CHILD = RIGHT_CHILD_OF(cnt);
    if( THIS->record[L_CHILD].target_cnt > THIS->record[L_CHILD+1].target_cnt && R_CHILD<THIS->record_cnt ){
      ++L_CHILD;
    }

    if( THIS->record[cnt].target_cnt>THIS->record[L_CHILD].target_cnt ){
      CMN_SWAP( THIS->record[PARENT_OF(cnt)].operation, THIS->record[cnt].operation);
      CMN_SWAP( THIS->record[PARENT_OF(cnt)].irqn, THIS->record[cnt].irqn);
      CMN_SWAP( THIS->record[PARENT_OF(cnt)].target_cnt, THIS->record[cnt].target_cnt);
    }else{
      break;
    }
  }
}

STATIC INLINE uint32_t cmn_interrupt_top( void){
  if(THIS->record_cnt==0){
    return UINT32_MAX;
  }

  return THIS->record[0].target_cnt;
}



#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
/**
 * @brief Disable the IRQn after few microseconds
 * @note  This function will enable the IRQn immediately
 * @todo
 *
*/
void cmn_interrupt_disable_irq_after( const cmnIRQn_t irq, const uint32_t ms){
  tCmnInterruptUnit item = {
    .operation = DISABLE,
    .irqn = irq,
    .target_cnt = ms
  };
  if(false==cmn_interrupt_push(&item)){
    //...//
  }

}

/**
 * @brief Enable the IRQn after few microseconds
 * @note  This function will disable the IRQn immediately
 * @todo
 *
*/
void cmn_interrupt_enable_irq_after( const cmnIRQn_t irq, const uint32_t ms){
  tCmnInterruptUnit item = {
    .operation = ENABLE,
    .irqn = irq,
    .target_cnt = ms
  };
  if(false==cmn_interrupt_push(&item)){
    //...//
  }
  
}

/**
 * @todo: Add timer cnt
*/
void cmn_interrupt_execute( void){
  if(cmn_interrupt_top()){
    //...//
    cmn_interrupt_pop();
  }
}



#define CMN_NVIC_PRIORITY_CRITICAL   4,0
#define CMN_NVIC_PRIORITY_IMPORTANT  8,0
#define CMN_NVIC_PRIORITY_NORMAL     9,0
#define CMN_NVIC_PRIORITY_CASUAL     10,0




void cmn_interrupt_init_priority( void){
#if 1
  HAL_NVIC_SetPriority(EXTI0_IRQn, CMN_NVIC_PRIORITY_IMPORTANT);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, CMN_NVIC_PRIORITY_IMPORTANT);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, CMN_NVIC_PRIORITY_CRITICAL);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, CMN_NVIC_PRIORITY_IMPORTANT);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, CMN_NVIC_PRIORITY_NORMAL);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
  
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, CMN_NVIC_PRIORITY_CASUAL);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, CMN_NVIC_PRIORITY_CASUAL);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

  HAL_NVIC_SetPriority(TIM2_IRQn, CMN_NVIC_PRIORITY_NORMAL);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  
  HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, CMN_NVIC_PRIORITY_NORMAL);
  HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);

  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, CMN_NVIC_PRIORITY_NORMAL);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
#endif
}

#endif



/** @defgroup ISR
 *  @ingroup Common
 *  Project Interrupt Service Routine Group
 *  @{
 */

/* ************************************************************************** */
/*                    Interrupt Service Routine Function                      */
/* ************************************************************************** */

void DEFAULT NMI_Handler( void){
  while(1){

  }
}

void DEFAULT HardFault_Handler( void){
  while(1){

  }
}

void DEFAULT MemManage_Handler( void){
  while(1){

  }
}

void BusFault_Handler( void){
  while(1){

  }
}

void UsageFault_Handler( void){
  while(1){

  }
}

void DebugMon_Handler( void){
  while(1){

  }
}

void PendSV_Handler( void){
  extern void FreeRTOS_FreeRTOS_PendSV_Handler(void);
  FreeRTOS_FreeRTOS_PendSV_Handler();
}

void SVC_Handler( void){
  extern void FreeRTOS_SVC_Handler(void);
  FreeRTOS_SVC_Handler();
}


/**
 * @note  1ms System Interrupt
 */
void SysTick_Handler( void){
  extern void FreeRTOS_SysTick_Handler( void);
  /* Clear overflow flag */
  SysTick->CTRL;

  if (metope.app.rtos.status == ON) {
    /* Call tick handler */
    FreeRTOS_SysTick_Handler();
  }
  HAL_IncTick();
}

void ADC_IRQHandler( void){}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  /**
   * @note: B5 QMI8658C INT1 Rising Edge Trigger
   */
  if(GPIO_Pin==GYRO_INT1_Pin){
    metope.dev.status.B5 = 1;
  }

  /**
   * @note: B6 QMI8658C INT2 Rising Edge Trigger
   */
  if(GPIO_Pin==GYRO_INT2_Pin){
    metope.dev.status.B6 = 1;
  }
  
  /**
   * @note: A9 Screen Touch Trigger
   */
  if(GPIO_Pin==TP_INT_Pin){
    bsp_led_toggle();
    metope.dev.status.A9 = 1;
  }
}

void EXTI9_5_IRQHandler(void){
  HAL_GPIO_EXTI_IRQHandler(GYRO_INT1_Pin);
  HAL_GPIO_EXTI_IRQHandler(GYRO_INT2_Pin);
  HAL_GPIO_EXTI_IRQHandler(TP_INT_Pin);
}

void TIM1_BRK_TIM9_IRQHandler(void){
#if (defined USE_REGISTER) && (USE_REGISTER==1)
  TIM9->SR = 0x00000000;
  CLEAR_BIT( TIM9->CR1, TIM_CR1_CEN);  // Disable timer
#else
  HAL_TIM_IRQHandler(&htim9);
#endif
  if(metope.app.rtos.status==ON){
    BaseType_t xHigherPriorityTaskWoken, xResult;
    xHigherPriorityTaskWoken = pdFALSE;
    xResult = xEventGroupSetBitsFromISR( metope.app.rtos.event._handle, CMN_EVENT_TIM9, &xHigherPriorityTaskWoken );
    if( xResult != pdFAIL ){
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
  }
}

void TIM1_UP_TIM10_IRQHandler(void){}     
void TIM1_TRG_COM_TIM11_IRQHandler(void){}
void TIM1_CC_IRQHandler(void){}           
void TIM2_IRQHandler(void){
  // extern volatile u32 gDummy;
  // gDummy = 0;
#if (defined USE_REGISTER) && (USE_REGISTER==1)
  TIM2->SR = 0x00000000;
  CLEAR_BIT( TIM2->CR1, TIM_CR1_CEN);  // Disable timer
#else
  HAL_TIM_IRQHandler(&htim2);
#endif
  metope.dev.status.tim2 = 1;
  if(metope.app.rtos.status==ON){
    BaseType_t xHigherPriorityTaskWoken, xResult;
    xHigherPriorityTaskWoken = pdFALSE;
    xResult = xEventGroupSetBitsFromISR( metope.app.rtos.event._handle, CMN_EVENT_TIM2, &xHigherPriorityTaskWoken );
    if( xResult != pdFAIL ){
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
  }
}              
void TIM3_IRQHandler(void){
  metope.dev.status.tim3 = 1;
} 


void TIM4_IRQHandler(void){}              
void I2C1_EV_IRQHandler(void){}           
void I2C1_ER_IRQHandler(void){}           
void I2C2_EV_IRQHandler(void){}           
void I2C2_ER_IRQHandler(void){}           
void SPI1_IRQHandler(void){}              

void SPI2_IRQHandler(void){
  HAL_SPI_IRQHandler(&hspi2);
}

void USART1_IRQHandler(void){}            

void DEFAULT EXTI15_10_IRQHandler( void){
  HAL_GPIO_EXTI_IRQHandler(KEY_L_Pin);
}

void RTC_Alarm_IRQHandler( void){}
void OTG_FS_WKUP_IRQHandler( void){}
void SDIO_IRQHandler( void){}
void TIM5_IRQHandler( void){}
void SPI3_IRQHandler( void){}
void DMA2_Stream0_IRQHandler( void){}
void DMA2_Stream1_IRQHandler( void){}
void DMA2_Stream4_IRQHandler( void){}
void WWDG_IRQHandler( void){}
void PVD_IRQHandler( void){}
void TAMP_STAMP_IRQHandler( void){}
void RTC_WKUP_IRQHandler( void){}
void FLASH_IRQHandler( void){}       
void RCC_IRQHandler( void){}         

void DEFAULT EXTI0_IRQHandler( void){
#if (defined USE_REGISTER) && (USE_REGISTER==1)
  if( READ_BIT(EXTI->PR, GPIO_PIN_0) ){
    EXTI->PR = GPIO_PIN_0;
  }
#else
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
#endif
  if(metope.app.rtos.status==ON){
    BaseType_t xHigherPriorityTaskWoken, xResult;
    xHigherPriorityTaskWoken = pdFALSE;
    xResult = xEventGroupSetBitsFromISR( metope.app.rtos.event._handle, CMN_EVENT_USER_KEY_M | CMN_EVENT_UPDATE_RTC, &xHigherPriorityTaskWoken );
    if( xResult != pdFAIL ){
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
  }
}

void DEFAULT EXTI1_IRQHandler( void){
  HAL_GPIO_EXTI_IRQHandler(KEY_R_Pin);
}


void EXTI2_IRQHandler( void){}       
void EXTI3_IRQHandler( void){}       
void EXTI4_IRQHandler( void){}       

/**
 * @note  QMI8658 I2C RX Interrupt
 */
void DMA1_Stream0_IRQHandler( void){
  extern DMA_HandleTypeDef hdma_i2c1_rx;
  HAL_DMA_IRQHandler(&hdma_i2c1_rx);
  metope.dev.status.i2c1 = IDLE;
}

/**
 * @note  QMI8658 I2C TX Interrupt
 */
void DMA1_Stream1_IRQHandler( void){
  // extern DMA_HandleTypeDef hdma_i2c1_tx;
  // HAL_DMA_IRQHandler(&hdma_i2c1_tx);
  // if(metope.app.rtos.status==ON){
  //   BaseType_t xHigherPriorityTaskWoken, xResult;
  //   xHigherPriorityTaskWoken = pdFALSE;
  //   xResult = xEventGroupSetBitsFromISR( metope.app.rtos.event._handle, CMN_EVENT_QMI8658_TX_CPLT, &xHigherPriorityTaskWoken );
  //   if( xResult != pdFAIL ){
  //     portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
  //   }
  // }
  // metope.dev.status.i2c1 = IDLE;
}

void DMA1_Stream2_IRQHandler( void){}
void DMA1_Stream3_IRQHandler( void){}

void DEFAULT DMA1_Stream4_IRQHandler(void){

#if (defined USE_REGISTER) && (USE_REGISTER==1)
  u32 tmp = DMA1->HISR;
#ifdef DEBUG_VERSION
  if( 0!=(tmp & (DMA_FLAG_TEIF0_4|DMA_FLAG_FEIF0_4|DMA_FLAG_DMEIF0_4)) ){
    /**
     * @todo: Change to the ASSERTION
     */
    while(1); // Transmission error
  }
#endif
  if(0!=(tmp & (DMA_FLAG_HTIF0_4))){
    DMA1->HIFCR = DMA_FLAG_HTIF0_4 << (4-4);
    DMA1_Stream4->CR &= ~(DMA_IT_HT);
    /**
     * @todo: Do something?
     */
  }else if(0!=(tmp & (DMA_FLAG_TCIF0_4))){
    DMA1->HIFCR = DMA_FLAG_TCIF0_4 << (4-4);
    DMA1_Stream4->CR &= ~(DMA_IT_TC);
    /**
     * @note: Full Transmission Complete Callback
     */
    cmn_callback_screen_spi_completed(&hspi2);
  }
  if(metope.app.rtos.status==ON){
    BaseType_t xHigherPriorityTaskWoken, xResult;
    xHigherPriorityTaskWoken = pdFALSE;
    xResult = xEventGroupSetBitsFromISR( metope.app.rtos.event._handle, CMN_EVENT_SCREEN_REFRESH_CPLT, &xHigherPriorityTaskWoken );
    if( xResult != pdFAIL ){
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
  }
  metope.dev.status.spi2 = IDLE;
#else
  HAL_DMA_IRQHandler(&hdma_spi2_tx);
  cmn_callback_screen_spi_completed(&hspi2);
#endif
}

void DMA1_Stream5_IRQHandler( void){}
void OTG_FS_IRQHandler( void){}
void DMA2_Stream5_IRQHandler( void){}
void DMA2_Stream6_IRQHandler( void){}
void DMA2_Stream7_IRQHandler( void){}
void USART6_IRQHandler( void){}
void I2C3_EV_IRQHandler( void){}
void I2C3_ER_IRQHandler( void){}
void FPU_IRQHandler( void){}
void SPI4_IRQHandler( void){}
void SPI5_IRQHandler( void){}



/** @} */ // end of ISR
/** @} */ // end of Common


#ifdef __cplusplus
}
#endif