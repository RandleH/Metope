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


#include "cmn_device.h"


#ifdef __cplusplus
extern "C"{
#endif


/** @defgroup Common 
 *  Project Common Group
 *  @{
 */

/** @defgroup ISR
 *  @ingroup Common
 *  Project Interrupt Service Routine Group
 *  @{
 */

/* ************************************************************************** */
/*                    Interrupt Service Routine Function                      */
/* ************************************************************************** */

void NMI_Handler( void){
  while(1){

  }
}

void HardFault_Handler( void){
  while(1){

  }
}

void MemManage_Handler( void){
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

}


void SysTick_Handler( void){

}

void ADC_IRQHandler( void){}        
void EXTI9_5_IRQHandler(void){}           
void TIM1_BRK_TIM9_IRQHandler(void){}     
void TIM1_UP_TIM10_IRQHandler(void){}     
void TIM1_TRG_COM_TIM11_IRQHandler(void){}
void TIM1_CC_IRQHandler(void){}           
void TIM2_IRQHandler(void){}              
void TIM3_IRQHandler(void){}              
void TIM4_IRQHandler(void){}              
void I2C1_EV_IRQHandler(void){}           
void I2C1_ER_IRQHandler(void){}           
void I2C2_EV_IRQHandler(void){}           
void I2C2_ER_IRQHandler(void){}           
void SPI1_IRQHandler(void){}              
void SPI2_IRQHandler(void){}
void USART1_IRQHandler(void){}            

void EXTI15_10_IRQHandler( void){
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
void DMA1_Stream5_IRQHandler( void){}
void WWDG_IRQHandler( void){}
void PVD_IRQHandler( void){}
void TAMP_STAMP_IRQHandler( void){}
void RTC_WKUP_IRQHandler( void){}
void FLASH_IRQHandler( void){}       
void RCC_IRQHandler( void){}         

void EXTI0_IRQHandler( void){
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI1_IRQHandler( void){
  HAL_GPIO_EXTI_IRQHandler(KEY_R_Pin);
}


void EXTI2_IRQHandler( void){}       
void EXTI3_IRQHandler( void){}       
void EXTI4_IRQHandler( void){}       
void DMA1_Stream0_IRQHandler( void){}
void DMA1_Stream1_IRQHandler( void){}
void DMA1_Stream2_IRQHandler( void){}
void DMA1_Stream3_IRQHandler( void){}
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