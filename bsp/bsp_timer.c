/**
 ******************************************************************************
 * @file    bsp_timer.c
 * @author  RandleH
 * @brief   Board Support Package Delivery - Timer
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



#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  #include "stm32f4xx_hal.h"
#endif
#include "bsp_timer.h"


#ifdef __cplusplus
extern "C"{
#endif



void bsp_timer_init_register( TIM_TypeDef *TIMx, uint32_t psc){
  TIMx->ARR = 10U;
  TIMx->PSC = psc;

  SET_BIT( TIMx->EGR, TIM_EGR_UG );   // Generate an update
  SET_BIT( TIMx->CR1, TIM_CR1_DIR);   // Down Counter
  SET_BIT( TIMx->CR1, TIM_CR1_URS);   // Update only when overflow
  SET_BIT( TIMx->CR1, TIM_CR1_OPM);   // One-pulse mode
  CLEAR_BIT( TIMx->CR1, TIM_CR1_CMS_0);
  CLEAR_BIT( TIMx->CR1, TIM_CR1_CMS_1);
  CLEAR_BIT( TIMx->CR1, TIM_CR1_ARPE);  // Auto-reload preload
  CLEAR_BIT( TIMx->CR1, TIM_CR1_CKD_0); // No clock division
  CLEAR_BIT( TIMx->CR1, TIM_CR1_CKD_1);
  TIMx->SR = 0x00000000;
  SET_BIT( TIMx->DIER, TIM_DIER_UIE);
  SET_BIT( TIMx->CR1, TIM_CR1_CEN);  // Enable timer

  u32 tmp;
  u8 cnt = 0xFF;
  do{
    // Wait until an update was generated
    tmp = TIMx->SR;
  }while( 0==READ_BIT( tmp, TIM_SR_UIF ) && --cnt!=0);
  
  TIMx->SR = ~(TIM_SR_UIF);

  CLEAR_BIT( TIMx->CR1, TIM_CR1_CEN);  // Disable timer
}


/**
 * @addtogroup MachineDependent
 */
void bsp_timer_init( void){
  
  __HAL_RCC_TIM2_CLK_ENABLE();
  __HAL_RCC_TIM9_CLK_ENABLE();
  
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);

  /**
   * @note: 
   *  f_TIMx = f_APB/( ${psc}+1 )
   *  MaxDelay = [(1<<NUM_CNT_BITS)-1] / f_TIMx
   */
#if (defined SYS_TARGET_STM32F411CEU6)
  bsp_timer_init_register( TIM2, 46875-1); /*!< APB1 = 96MHz; f_TIM2 = 2.048kHz; CNT[0:15]; MaxDelay = 32000ms  */
  bsp_timer_init_register( TIM9, 96-1);    /*!< APB1 = 96MHz; f_TIM9 = 1MHz;     CNT[0:15]; MaxDelay = 65.536ms */
#else
  #error "Unimplemented"
#endif

}


/**
 * @addtogroup MachineDependent
 * @brief Enable the Timer(TIM2) Counting
 * @note Timer frequency: f_TIM2 = 2.048kHz
 * @param [in] us - Milliseconds | MaxDelay = 32000ms
 */
void bsp_tim2_delay(u16 ms){
  TIM2->ARR = ms * 2048 / 1000;
  SET_BIT( TIM2->EGR, TIM_EGR_UG);
  SET_BIT( TIM2->CR1, TIM_CR1_CEN);
}


/**
 * @addtogroup MachineDependent
 * @brief Enable the Timer(TIM9) Counting
 * @note Timer frequency: f_TIM9 = 1MHz
 * @param [in] us - Microseconds | MaxDelay = 65535us
 */
void bsp_tim9_delay(u16 us){
  TIM9->ARR = us;
  SET_BIT( TIM9->EGR, TIM_EGR_UG);
  SET_BIT( TIM9->CR1, TIM_CR1_CEN);
}

#ifdef __cplusplus
}
#endif