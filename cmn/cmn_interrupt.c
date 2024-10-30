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
 * @todo
 *  Add timer cnt
*/
void cmn_interrupt_execute( void){
  if(cmn_interrupt_top()){
    //...//
    cmn_interrupt_pop();
  }
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

}


/**
 * @note  1ms System Interrupt
 */
void DEFAULT SysTick_Handler( void){
  HAL_IncTick();
}

void ADC_IRQHandler( void){}

void EXTI9_5_IRQHandler(void){
  HAL_GPIO_EXTI_IRQHandler(TP_INT_Pin);
}

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
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void DEFAULT EXTI1_IRQHandler( void){
  HAL_GPIO_EXTI_IRQHandler(KEY_R_Pin);
}


void EXTI2_IRQHandler( void){}       
void EXTI3_IRQHandler( void){}       
void EXTI4_IRQHandler( void){}       
void DMA1_Stream0_IRQHandler( void){}
void DMA1_Stream1_IRQHandler( void){}
void DMA1_Stream2_IRQHandler( void){}
void DMA1_Stream3_IRQHandler( void){}

void DEFAULT DMA1_Stream4_IRQHandler(void){
  HAL_DMA_IRQHandler(&hdma_spi2_tx);
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