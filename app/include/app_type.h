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
/*                              Headfile Guards                               */
/* ************************************************************************** */
#ifndef APP_TYPE_H
#define APP_TYPE_H


/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */


#define APP_CFG_TASK_SCREEN_FRESH_STACK_SIZE (2048U)
#define APP_CFG_TASK_CLOCK_UI_STACK_SIZE     ( 512U)
#define APP_CFG_TASK_SCREEN_ONOFF_STACK_SIZE ( 256U)
#define APP_CFG_TASK_CMD_BOX_STACK_SIZE      (1024U)


#define APP_IDLE_CLOCK       (1<<0)
#define APP_IDLE_BATTERY     
#define APP_IDLE_SYSINFO     



#endif
