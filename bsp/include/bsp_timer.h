/**
 ******************************************************************************
 * @file    bsp_timer.h
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

#include "cmn_type.h"

#ifndef BSP_TIMER_H
#define BSP_TIMER_H 

#ifdef __cplusplus
extern "C"{
#endif

void bsp_timer_init( void);
void bsp_tim2_delay(u16 ms);
void bsp_tim9_delay(u16 us);

#ifdef __cplusplus
}
#endif

#endif