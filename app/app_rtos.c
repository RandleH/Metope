/**
 ******************************************************************************
 * @file    app_rtos.c
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


/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include "FreeRTOS.h"
#include "global.h"
#include "app_rtos.h"
#include "cmn_type.h"


#ifdef __cplusplus
extern "C"{
#endif

void app_rtos_start( void){
  metope.app.rtos.status = ON;
  vTaskStartScheduler();
}

void app_rtos_stop( void){
  metope.app.rtos.status = OFF;
  vTaskEndScheduler();
}


#ifdef __cplusplus
}
#endif