/**
 ******************************************************************************
 * @file    bsp_battery.h
 * @author  RandleH
 * @brief   Board Support Package Delivery - Battery
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 RandleH.
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

#ifndef BSP_BATTERY_H
#define BSP_BATTERY_H

#ifdef __cplusplus
extern "C"{
#endif


uint8_t bsp_battery_measure(void);


#ifdef __cplusplus
}
#endif

#endif

/* ********************************** EOF *********************************** */
