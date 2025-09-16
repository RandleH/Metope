/**
 ******************************************************************************
 * @file    app_rtos.h
 * @author  RandleH
 * @brief   Application - FreeRTOS Application Interface
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


#ifndef APP_RTOS_H
#define APP_RTOS_H

#include "app_type.h"

#ifdef __cplusplus
extern "C"{
#endif

void app_rtos_start( void);
void app_rtos_stop( void);
void app_rtos_checkpoint(void *param) RTOSTHREAD;

#ifdef __cplusplus
}
#endif

#endif