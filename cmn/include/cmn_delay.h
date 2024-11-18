/**
 ******************************************************************************
 * @file    cmn_delay.h
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



#include "cmn_type.h"


#ifndef CMN_DELAY_H
#define CMN_DELAY_H

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @todo: Consider removing this function
 */
void cmn_timer_delay(u32 ms);

cmnBoolean_t cmn_tim2_sleep(u16 ms);
cmnBoolean_t cmn_tim9_sleep(u16 us);

#ifdef __cplusplus
}
#endif

#endif
