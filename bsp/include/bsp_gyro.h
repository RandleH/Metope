/**
 ******************************************************************************
 * @file    bsp_gyro.h
 * @author  RandleH
 * @brief   Board Support Package Delivery - QMI8658C
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

#ifndef BSP_GYRO_H
#define BSP_GYRO_H

#ifdef __cplusplus
extern "C"{
#endif

void bsp_gyro_init(void);
void bsp_gyro_switch( cmnBoolean_t on_off);
void bsp_gyro_reset( void);
u8 bsp_gyro_get_chip_id(void);

#ifdef __cplusplus
}
#endif


#endif
