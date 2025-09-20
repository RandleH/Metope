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
#include "cmn_type.h"

#ifndef BSP_TYPE_H
#define BSP_TYPE_H


#define BSP_SCREEN_WIDTH          (240)
#define BSP_SCREEN_HEIGHT         (240)

#if (BSP_SCREEN_WIDTH<=UINT8_MAX) && (BSP_SCREEN_HEIGHT<=UINT8_MAX)
typedef u8 bspScreenCood_t;
#else
typedef u16 bspScreenCood_t;
#endif

#define BSP_SCREEN_MIN_BRIGHTNESS       (0U)
#define BSP_SCREEN_MAX_BRIGHTNESS       (2048U-1)
#define BSP_SCREEN_DEFAULT_BRIGHTNESS   (1024U)
#define BSP_SCREEN_DEFAULT_REFREASHRATE (10)

#define BSP_SCREEN_USE_HARDWARE_NSS     1

#define BSP_CFG_UART_TX_BUF_SIZE        256
#define BSP_CFG_UART_RX_BUF_SIZE        32

#ifdef __cplusplus
extern "C"{
#endif




#ifdef __cplusplus
}
#endif


#endif
