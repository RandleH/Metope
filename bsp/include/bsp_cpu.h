/**
 ******************************************************************************
 * @file    bsp_cpu.h
 * @author  RandleH
 * @brief   Board Support Package Delivery - CPU
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

#ifndef BSP_CPU_H
#define BSP_CPU_H

#ifdef __cplusplus
extern "C"{
#endif

void bsp_cpu_clock_init( void);
const u8* bsp_cpu_chip_uuid( size_t *len);

#ifdef __cplusplus
}
#endif


#endif