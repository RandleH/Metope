




/**
 ******************************************************************************
 * @file    cmn_delay.c
 * @author  RandleH
 * @brief   Common Program - Delay Program
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

#include "global.h"
#include "cmn_type.h"
#include "cmn_delay.h"
#include "bsp_timer.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief
 * @addtogroup MachineDependent
 */
void cmn_timer_delay(u32 ms){
  HAL_Delay(ms);
}

/**
 * @brief TIM2 based Non-block sleep given the microsecond
 * @note  The current process will pause until wake up
 *        TIM2 was runnning on 2.048KHz. The maximum delay ms is `32000`
 * @param [in]  ms - Microseconds
 * @return  Return `BUSY` when timer is currently unavailable
 *          Return `SUCCESS` when finished.
 */
cmnBoolean_t cmn_tim2_sleep(u16 ms){
  /* Config Timer Register */
  bsp_tim2_delay(ms);
  
  /* Wait until event was called */
  if(metope.app.rtos.status==ON){
    EventBits_t uxBits = xEventGroupWaitBits( metope.app.rtos.event._handle, CMN_EVENT_TIM2, pdTRUE, pdFALSE, portMAX_DELAY);
  }else{
    u32 tmp;
    do{
      // Wait until an update was generated
      tmp = TIM2->SR;
    }while( 0==READ_BIT( tmp, TIM_SR_UIF ));
    
    TIM2->SR = ~(TIM_SR_UIF);
  }
  
  return SUCCESS;
}


/**
 * @brief TIM9 based Non-block sleep given the microsecond
 * @note  The current process will pause until wake up
 *        TIM9 was runnning on 1MHz. The maximum delay ms is `65535`
 * @param [in]  us - Microseconds
 * @return  Return `BUSY` when timer is currently unavailable
 *          Return `SUCCESS` when finished.
 */
cmnBoolean_t cmn_tim9_sleep(u16 us){
  /* Config Timer Register */
  bsp_tim9_delay(us);
  
  /* Wait until event was called */
  if(metope.app.rtos.status==ON){
    xEventGroupWaitBits( metope.app.rtos.event._handle, CMN_EVENT_TIM9, pdTRUE, pdFALSE, portMAX_DELAY);
  }else{
    u32 tmp;
    do{
      // Wait until an update was generated
      tmp = TIM9->SR;
    }while( 0==READ_BIT( tmp, TIM_SR_UIF ));
    
    TIM9->SR = ~(TIM_SR_UIF);
  }
  
  return SUCCESS;
}


#ifdef __cplusplus
}
#endif