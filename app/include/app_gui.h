/**
 ******************************************************************************
 * @file    app_gui.h
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
#include "lvgl.h"
#include "cmn_type.h"
#include "app_type.h"


/* ************************************************************************** */
/*                              Headfile Guards                               */
/* ************************************************************************** */
#ifndef APP_GUI_H
#define APP_GUI_H


#ifdef __cplusplus
extern "C"{
#endif

void app_gui_switch( AppGuiClockEnum_t x);
void app_clock_gui_main(void*param) RTOSTHREAD;
void app_gui_update_modulo(tAppGuiClockParam *pClient) RTOSIDLE;

#ifdef __cplusplus
}
#endif



#endif