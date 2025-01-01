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
uint32_t cmn_math_largest_pow2(uint32_t x);
uint32_t cmn_math_pow10(uint8_t x);
uint32_t cmn_math_largest_pow10(uint32_t x);
uint8_t cmn_math_count_dec_digits( uint32_t x);
#ifdef __cplusplus
}
#endif

#endif
/* ********************************** EOF *********************************** */
