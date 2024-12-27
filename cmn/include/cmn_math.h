/**
 ******************************************************************************
 * @file    cmn_math.h
 * @author  RandleH
 * @brief   Common Program - Math
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
#include "device.h"


#ifndef CMN_MATH_H
#define CMN_MATH_H



#ifdef __cplusplus
extern "C"{
#endif


uint32_t cmn_math_gcd( uint32_t a, uint32_t b);


#ifdef __cplusplus
}
#endif

#endif