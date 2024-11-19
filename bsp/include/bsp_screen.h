/**
 ******************************************************************************
 * @file    bsp_screen.h
 * @author  RandleH
 * @brief   Board Support Package Delivery - Screen
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
#include "cmn_device.h"
#include "bsp_type.h"


#ifndef BSP_SCREEN_H
#define BSP_SCREEN_H

#ifdef __cplusplus
extern "C"{
#endif



cmnBoolean_t bsp_screen_init( void);
void bsp_screen_on(void);
void bsp_screen_off(void);
void bsp_screen_smooth_on(void);
void bsp_screen_smooth_off(void);
void bsp_screen_set_bright( bspScreenBrightness_t value);
void bsp_screen_fill( const bspScreenPixel_t color, bspScreenCood_t xs, bspScreenCood_t ys, bspScreenCood_t xe, bspScreenCood_t ye);
void bsp_screen_refresh( const bspScreenPixel_t *buf, bspScreenCood_t xs, bspScreenCood_t ys, bspScreenCood_t xe, bspScreenCood_t ye);

#ifdef __cplusplus
}
#endif


#endif




