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
#include "lvgl.h"

/* ************************************************************************** */
/*                              Headfile Guards                               */
/* ************************************************************************** */
#ifndef APP_CLOCK_H
#define APP_CLOCK_H


#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
  lv_obj_t *pScreen;
  lv_obj_t *pPinHour;
  lv_obj_t *pPinMinute;

  cmnDateTime_t time;

  TimerHandle_t _idle_task_timer;

  struct{
    SemaphoreHandle_t  _semphr;
    void              *p_anything;
  }customized;
} tAppGuiClockParam;

/**
 * @brief List of all supported clock ui
 */
typedef enum{
  kAppGuiClock_None        = 255,
  kAppGuiClock_ClockModern = 0,
  kAppGuiClock_NANA        = 1,
  kAppGuiClock_LVVVW       = 2,
  NUM_OF_AppGuiClock
} AppGuiClockEnum_t;

typedef struct stAppClockFunc {
  void (*init)(tAppGuiClockParam *);
  void (*set_time)(tAppGuiClockParam *, uint32_t);
  void (*inc_time)(tAppGuiClockParam *, uint32_t);
  void (*idle)(tAppGuiClockParam *);
  void (*deinit)(tAppGuiClockParam *);
} tAppClockFunc;

typedef struct stAppClock{
  TaskHandle_t      _handle;
  AppGuiClockEnum_t style;
  tAppGuiClockParam param;
  tAppClockFunc     func;
} tAppClock;



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
