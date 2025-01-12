/**
 ******************************************************************************
 * @file    app_type.h
 * @author  RandleH
 * @brief   Application Program - Global Declaration
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
#include "lvgl.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"

/* ************************************************************************** */
/*                              Headfile Guards                               */
/* ************************************************************************** */
#ifndef APP_TYPE_H
#define APP_TYPE_H

#define APP_CFG_TASK_SCREEN_FRESH_STACK_SIZE (2048U)
#define APP_CFG_TASK_CLOCK_UI_STACK_SIZE     (512U)
#define APP_CFG_TASK_SCREEN_ONOFF_STACK_SIZE (256U)


#define APP_IDLE_CLOCK       (1<<0)
#define APP_IDLE_BATTERY     
#define APP_IDLE_SYSINFO     

#define RTOSTHREAD      /*!< Running on a FreeRTOS Thread */
#define RTOSIDLE        /*!< Running on a FreeRTOS Idle Thread */
#define RTOSTIMER       /*!< Running on a FreeRTOS Timer Callback */

typedef enum AppTaskPriority{
  kAppPriority_VERY_IMPORTANT = 50,
  kAppPriority_IMPORTANT      = 40,
  kAppPriority_INTERMEDIATE   = 30,
  kAppPriority_NORMAL         = 20,
  kAppPriority_DOCUMENTATION  = 5
}AppTaskPriority_t;

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
  kAppGuiClock_None   = 255,
  kAppGuiClock_Clock1 = 0,
  kAppGuiClock_NANA   = 1,
  kAppGuiClock_LVVVW  = 2
} AppGuiClockEnum_t;


#endif