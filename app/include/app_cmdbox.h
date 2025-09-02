/**
 ******************************************************************************
 * @file    app_cmdbox.h
 * @author  RandleH
 * @brief   Application Program - Command Box
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
#ifndef APP_CMDBOX_H
#define APP_CMDBOX_H


#ifdef __cplusplus
extern "C"{
#endif

void app_cmdbox_main(void *param) RTOSTHREAD;
void app_cmdbox_idle(void *param) RTOSIDLE;

#ifdef __cplusplus
}
#endif



#endif
/* ********************************** EOF *********************************** */
