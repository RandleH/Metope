




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

#include "cmn_delay.h"


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






#ifdef __cplusplus
}
#endif