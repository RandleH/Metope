/**
 ******************************************************************************
 * @file    bsp_type.h
 * @author  RandleH
 * @brief   Board Support Package Delivery - Type Declaration
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

#include <stdint.h>
#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  
#endif

#ifndef BSP_TYPE_H
#define BSP_TYPE_H


#define BSP_SCREEN_WIDTH          (240)
#define BSP_SCREEN_HEIGHT         (240)

#define BSP_SCREEN_MIN_BRIGHTNESS     (0U)
#define BSP_SCREEN_MAX_BRIGHTNESS     (2048U-1)
#define BSP_SCREEN_DEFAULT_BRIGHTNESS (1024U)


#ifdef __cplusplus
extern "C"{
#endif




typedef uint16_t  bspScreenBrightness_t;


#ifdef __cplusplus
}
#endif


#endif
