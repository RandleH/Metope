/**
 ******************************************************************************
 * @file    cmn_type.h
 * @author  RandleH
 * @brief   Common Program - Type Declaration
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
  #include "stm32f4xx_hal.h"
#endif

#ifndef CMN_TYPE_H
#define CMN_TYPE_H




#define DISABLE   (0U)
#define ENABLE    (1U)

#define OFF       (0U)
#define ON        (1U)

#define IDLE      (0U)
#define BUSY      (1U)

#define NO        (0U)
#define YES       (1U)

#define NULLABLE

#define FAILED    (0U)
#define PASSED    (1U)

#define SUCCESS   (0U)
#define ERROR     (1U)


#ifdef TEST_ONLY
  #define STATIC
  #define INLINE
#else
  #define STATIC static
  #define INLINE inline
#endif

#define DEFAULT __attribute__((weak))



#ifdef __cplusplus
extern "C"{
#endif

typedef uint8_t cmnBoolean_t;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef volatile u8  vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;

#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
typedef IRQn_Type cmnIRQn_t;
#endif


typedef union cmnDateTime_t{
  struct{
    uint32_t  year   : 6;    /*!< Distance from 2024 +- 32 */
    uint32_t  month  : 4;
    uint32_t  day    : 5;
    uint32_t  hour   : 5;
    uint32_t  minute : 6;
    uint32_t  second : 6;
  };
  uint32_t word;
}cmnDateTime_t;




#ifdef __cplusplus
}
#endif  // extern C
#endif  // CMN_TEST_H

