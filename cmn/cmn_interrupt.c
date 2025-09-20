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

/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include <stdbool.h>
#include "device.h"
#include "global.h"
#include "assert.h"
#include "cmn_type.h"
#include "cmn_interrupt.h"
#include "cmn_utility.h"
#include "cmn_callback.h"
#include "bsp_led.h"
#include "bsp_screen.h"
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



#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || (defined EMULATOR_STM32F405RGT6)
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

/**
 * @note
 * The priority grouping bits length. This parameter can be one of the following values:
 *  - NVIC_PRIORITYGROUP_0: 0 bits for preemption priority 4 bits for subpriority
 *  - NVIC_PRIORITYGROUP_1: 1 bits for preemption priority 3 bits for subpriority
 *  - NVIC_PRIORITYGROUP_2: 2 bits for preemption priority 2 bits for subpriority
 *  - NVIC_PRIORITYGROUP_3: 3 bits for preemption priority 1 bits for subpriority
 *  - NVIC_PRIORITYGROUP_4: 4 bits for preemption priority 0 bits for subpriority
 * @note
 *  NVIC_PRIORITYGROUP_4 was used by default
 */
#define CMN_NVIC_PRIORITY_CRITICAL    4,0
#define CMN_NVIC_PRIORITY_IMPORTANT   8,0
#define CMN_NVIC_PRIORITY_NORMAL      9,0
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

  /* USART2 interrupt Init */
  HAL_NVIC_SetPriority(USART2_IRQn, CMN_NVIC_PRIORITY_NORMAL);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
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
  volatile uint8_t flag = true;
  while(flag){
    __NOP();
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

  if (metope.rtos.status->running[0]) {
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
    metope.bsp.status->B5[0] = 1;
  }

  /**
   * @note: B6 QMI8658C INT2 Rising Edge Trigger
   */
  if(GPIO_Pin==GYRO_INT2_Pin){
    metope.bsp.status->B6[0] = 1;
  }
  
  /**
   * @note: A9 Screen Touch Trigger
   */
  if(GPIO_Pin==TP_INT_Pin){
    metope.bsp.status->A9[0] = 1;
  }
}

void EXTI9_5_IRQHandler(void){
  HAL_GPIO_EXTI_IRQHandler(GYRO_INT1_Pin);
  HAL_GPIO_EXTI_IRQHandler(GYRO_INT2_Pin);
  HAL_GPIO_EXTI_IRQHandler(TP_INT_Pin);
}


uint32_t TIM9_FLAG = 0;
void TIM1_BRK_TIM9_IRQHandler(void){
#if 1
  TIM9->SR = 0x00000000;
  CLEAR_BIT( TIM9->CR1, TIM_CR1_CEN);  // Disable timer
#else
  HAL_TIM_IRQHandler(&htim9);
#endif
  metope.bsp.status->tim9[0] = 1;
  TIM9_FLAG = 1;
  if(metope.rtos.status->running[0]){
    BaseType_t xHigherPriorityTaskWoken, xResult;
    xHigherPriorityTaskWoken = pdFALSE;
    xResult = xEventGroupSetBitsFromISR( metope.rtos.event._handle, CMN_EVENT_TIM9, &xHigherPriorityTaskWoken );
    if( xResult != pdFAIL ){
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
  }
}

void TIM1_UP_TIM10_IRQHandler(void){}     
void TIM1_TRG_COM_TIM11_IRQHandler(void){}
void TIM1_CC_IRQHandler(void){}           
void TIM2_IRQHandler(void){
#if 1
  TIM2->SR = 0x00000000;
  CLEAR_BIT( TIM2->CR1, TIM_CR1_CEN);  // Disable timer
#else
  HAL_TIM_IRQHandler(&htim2);
#endif
  metope.bsp.status->tim2[0] = 1;
  if(metope.rtos.status->running[0]){
    BaseType_t xHigherPriorityTaskWoken, xResult;
    xHigherPriorityTaskWoken = pdFALSE;
    xResult = xEventGroupSetBitsFromISR( metope.rtos.event._handle, CMN_EVENT_TIM2, &xHigherPriorityTaskWoken );
    if( xResult != pdFAIL ){
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
  }
}              
void TIM3_IRQHandler(void){}


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

/**
 * @brief
 * @note Only DMA or Idle Interrupt, HAL will call this function.
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  tBspUart *p_uart = &metope.bsp.uart;
  
  if(huart->Instance == USART2) {
    p_uart->rx_idx      = 0;
    p_uart->rx_status.has_new_msg = 1;
    HAL_UART_Receive_IT( huart, (uint8_t*)&p_uart->rx_buf[0], BSP_CFG_UART_RX_BUF_SIZE);
  }
}

/**
 * @brief
 * @note Each time when the transfer count reaches the buffer size, this function will be called.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  tBspUart *p_uart = &metope.bsp.uart;
  p_uart->rx_status.is_locked = 1;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  tBspUart *p_uart = &metope.bsp.uart;
  p_uart->rx_status.error_code = huart->ErrorCode;
}

void USART2_IRQHandler(void) {
  // HAL_UART_IRQHandler(&huart2);
  tBspUart *p_uart = &metope.bsp.uart;
  volatile uint32_t REG_USART2_SR = USART2->SR;
  volatile uint32_t REG_USART2_DR = USART2->DR;
  
  if (REG_USART2_SR & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_NE)) {
    p_uart->rx_status.error_code = (uint8_t)(REG_USART2_SR & 0x1F);
    p_uart->rx_idx = BSP_CFG_UART_RX_BUF_SIZE;
  }
  else if (REG_USART2_SR & USART_SR_RXNE) {
    if (likely(!p_uart->rx_status.is_overflowed)) {
      p_uart->rx_buf[p_uart->rx_idx++] = (char)REG_USART2_DR;
      if (p_uart->rx_idx == BSP_CFG_UART_RX_BUF_SIZE) {
        p_uart->rx_status.is_overflowed = 1;
      }
      if (REG_USART2_DR == '\n' || REG_USART2_DR == '\r') {
        p_uart->rx_status.has_new_msg = 1;
      }
    }
  }

  /* Always set the '\0' after the string */
  p_uart->rx_buf[p_uart->rx_idx] = '\0';

  if(metope.rtos.status->running[0]){
    BaseType_t xHigherPriorityTaskWoken, xResult;
    xHigherPriorityTaskWoken = pdFALSE;
    xResult = xEventGroupSetBitsFromISR( metope.rtos.event._handle, CMN_EVENT_UART_INPUT, &xHigherPriorityTaskWoken );
    if( xResult != pdFAIL ){
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
  }
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
  cmn_callback_user_key_detected(KEY_M_Pin);
}

void DEFAULT EXTI1_IRQHandler( void){
  cmn_callback_user_key_detected(KEY_R_Pin);
}

void DEFAULT EXTI15_10_IRQHandler( void){
  cmn_callback_user_key_detected(KEY_L_Pin);
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
  metope.bsp.status->i2c1[0] = IDLE;
}

/**
 * @note  QMI8658 I2C TX Interrupt
 */
void DMA1_Stream1_IRQHandler( void){
  // extern DMA_HandleTypeDef hdma_i2c1_tx;
  // HAL_DMA_IRQHandler(&hdma_i2c1_tx);
  // if(metope.rtos.status->running[0]){
  //   BaseType_t xHigherPriorityTaskWoken, xResult;
  //   xHigherPriorityTaskWoken = pdFALSE;
  //   xResult = xEventGroupSetBitsFromISR( metope.rtos.event._handle, CMN_EVENT_QMI8658_TX_CPLT, &xHigherPriorityTaskWoken );
  //   if( xResult != pdFAIL ){
  //     portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
  //   }
  // }
  // metope.bsp.status->i2c1[0] = IDLE;
}

void DMA1_Stream2_IRQHandler( void){}
void DMA1_Stream3_IRQHandler( void){}

void DEFAULT DMA1_Stream4_IRQHandler(void){

#if 1
  u32 tmp = DMA1->HISR;

  ASSERT( 0==(tmp & (DMA_FLAG_TEIF0_4|DMA_FLAG_FEIF0_4|DMA_FLAG_DMEIF0_4)), "DMA1S4 Error");
  
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
  if(metope.rtos.status->running[0]){
    BaseType_t xHigherPriorityTaskWoken, xResult;
    xHigherPriorityTaskWoken = pdFALSE;
    xResult = xEventGroupSetBitsFromISR( metope.rtos.event._handle, CMN_EVENT_SCREEN_REFRESH_CPLT, &xHigherPriorityTaskWoken );
    if( xResult != pdFAIL ){
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
  }
  metope.bsp.status->spi2[0] = IDLE;
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