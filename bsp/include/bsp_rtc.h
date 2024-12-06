/**
 ******************************************************************************
 * @file    bsp_rtc.h
 * @author  RandleH
 * @brief   Board Support Package Delivery - Real Time Clock Module
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


#ifndef BSP_RTC_H
#define BSP_RTC_H

#include "cmn_type.h"

#ifdef __cplusplus
extern "C"{
#endif


void bsp_rtc_init(void);
cmnDateTime_t bsp_rtc_get_time(void);
void bsp_rtc_set_time( const cmnDateTime_t t);

#ifdef __cplusplus
}
#endif

#endif

