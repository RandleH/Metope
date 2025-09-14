/**
 ******************************************************************************
 * @file    app_clock.h
 * @author  RandleH
 * @brief   Application Program - GUI Program
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
#include "cmn_type.h"
#include "app_type.h"


/* ************************************************************************** */
/*                              Headfile Guards                               */
/* ************************************************************************** */
#ifndef APP_CLOCK_H
#define APP_CLOCK_H


#ifdef __cplusplus
extern "C"{
#endif

void app_clock_main(void *param) RTOSTHREAD;
void app_clock_idle(void *param) RTOSIDLE;

#if (defined UNIT_TEST) && (UNIT_TEST==1)
extern void         app_clock_gui_switch         (AppGuiClockEnum_t x);
extern void         app_clock_idle_timer_callback(xTimerHandle xTimer);
extern xTimerHandle app_clock_idle_timer_regist  (void);
extern void         app_clock_idle_timer_unregist(xTimerHandle xTimer);
#endif

#ifdef __cplusplus
}
#endif



#endif
/* ********************************** EOF *********************************** */
