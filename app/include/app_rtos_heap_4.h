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

#include <stdlib.h>


#ifndef APP_RTOS_HEAP4_H
#define APP_RTOS_HEAP4_H



#ifdef __cplusplus
extern "C"{
#endif

void *pvPortMalloc( size_t xWantedSize );
void vPortFree( void *pv );
void *pvPortRealloc( void *pv, size_t xWantedSize );

#ifdef __cplusplus
}
#endif

#endif
