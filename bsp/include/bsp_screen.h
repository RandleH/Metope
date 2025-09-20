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
#ifndef BSP_SCREEN_H
#define BSP_SCREEN_H


/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */


#ifdef __cplusplus
extern "C"{
#endif


typedef uint16_t bspScreenBrightness_t;
typedef uint8_t  bspScreenRotate_t;
typedef uint16_t bspScreenPixel_t;

typedef union stBspScreenStatusBitmap {
  struct {
    uint16_t is_disp_off : 1;
    uint16_t reserved    : 15;
  };
  uint16_t word;
} tBspScreenStatusBitmap;

typedef union stBspScreenStatusBitbandmap {
  uint16_t is_disp_off [1];
  uint16_t reserved    [15];
} tBspScreenStatusBitbandmap;

typedef struct stBspScreen{
  bspScreenBrightness_t      brightness;
  bspScreenRotate_t          rotation;
  uint32_t                   refresh_rate_ms;
  tBspScreenStatusBitmap     _status;
  tBspScreenStatusBitbandmap *status;
} tBspScreen;


cmnBoolean_t bsp_screen_init( void);
void bsp_screen_on(void);
void bsp_screen_off(void);
void bsp_screen_smooth_on(void);
void bsp_screen_smooth_off(void);
void bsp_screen_set_bright( bspScreenBrightness_t value);
void bsp_screen_rotate( bspScreenRotate_t delta, uint8_t cw_ccw);
void bsp_screen_fill( const bspScreenPixel_t color, bspScreenCood_t xs, bspScreenCood_t ys, bspScreenCood_t xe, bspScreenCood_t ye);
void bsp_screen_refresh( const bspScreenPixel_t *buf, bspScreenCood_t xs, bspScreenCood_t ys, bspScreenCood_t xe, bspScreenCood_t ye);

#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || (defined EMULATOR_STM32F405RGT6)
void bsp_screen_main(void *param) RTOSTHREAD;
void bsp_screen_onoff(void *param) RTOSTHREAD;
#endif

#ifdef __cplusplus
}
#endif


#endif




