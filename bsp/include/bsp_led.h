/**
 ******************************************************************************
 * @file    bsp_led.h
 * @author  RandleH
 * @brief   Board Support Package Delivery - LED
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





#include "cmn_type.h"


#ifndef BSP_LED_H
#define BSP_LED_H

#ifdef __cplusplus
extern "C"{
#endif

void bsp_led__on( void);
void bsp_led__off( void);
void bsp_led__toggle( void);
void bsp_led__switch( cmnBoolean_t status);

#ifdef __cplusplus
}
#endif


#endif




