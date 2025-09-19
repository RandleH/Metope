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



/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include "device.h"
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
cmnBoolean_t cmn_tim2_sleep(u16 ms, cmnBoolean_t async_mode){
  if(metope.bsp.status.tim2==1){
    return BUSY;
  }
  /* Config Timer Register */
  bsp_tim2_delay(ms);
  /* Wait until event was called */
  if(async_mode){
    xEventGroupWaitBits( metope.rtos.event._handle, CMN_EVENT_TIM2, pdTRUE, pdFALSE, portMAX_DELAY);
  }else{
    while(metope.bsp.status.tim2==0);
  }
  metope.bsp.status.tim2 = 0;
  return SUCCESS;
}


/**
 * @brief TIM9 based Non-block sleep given the microsecond
 * @note  The current process will pause until wake up
 *        TIM9 was runnning on 1MHz. The maximum delay ms is `65535`
 * @param [in] us         - Microseconds
 * @param [in] async_mode - `ENABLE` | `DISABLE` If enabled, context switch is allowed.
 * @todo  Dead loop occurs when rtos was off !!!
 * @return  Return `BUSY` when timer is currently unavailable
 *          Return `SUCCESS` when finished.
 */
cmnBoolean_t cmn_tim9_sleep(u16 us, cmnBoolean_t async_mode){
  extern uint32_t TIM9_FLAG;
  if(TIM9_FLAG==1){
    return BUSY;
  }
  /* Config Timer Register */
  bsp_tim9_delay(us);
  /* Wait until event was called */
  if(async_mode){
    xEventGroupWaitBits( metope.rtos.event._handle, CMN_EVENT_TIM9, pdTRUE, pdFALSE, portMAX_DELAY);
  }else{
    while(TIM9_FLAG==0);
  }
  TIM9_FLAG = 0;
  return SUCCESS;
}


#ifdef __cplusplus
}
#endif