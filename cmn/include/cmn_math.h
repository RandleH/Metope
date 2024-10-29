/**
 ******************************************************************************
 * @file    cmn_math.h
 * @author  RandleH
 * @brief   Common Program - Math Library
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
  #include "arm_math.h"
#endif


#ifndef CMN_MATH_H
#define CMN_MATH_H



#ifdef __cplusplus
extern "C"{
#endif

#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  #define CMN_BIT_EXT_U8( u8_x, pos, len) \
  // ({q7_t dst; arm_clip_q7(&(u8_x), &dst, (pos), ((pos)+(len)-1), 1); dst;})
  #define CMN_BIT_EXT_U16( u16_x, pos, len) \
  // ({uint16_t dst; arm_clip_q15(&(u16_x), &dst, (pos), ((pos)+(len)-1), 1); dst;})

  #define CMN_CLZ_U32(u32_x)  __CLZ(u32_x)
#else

#endif


uint32_t cmn_math_gcd( uint32_t a, uint32_t b);


#ifdef __cplusplus
}
#endif

#endif