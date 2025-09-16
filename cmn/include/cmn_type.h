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


/**
 * @ref https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
 */
#if defined(__GNUC__) || defined(__GNUG__)
  #pragma GCC diagnostic push
  // #pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#elif defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#elif defined(__ICCARM__)
  #pragma diag_suppress=203
#elif defined(_MSC_VER)

#endif


#include <stdint.h>
#include "device.h"

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


#ifdef UNIT_TEST
  #define STATIC
  #define INLINE
#else
  #define STATIC static
  #define INLINE inline
#endif

#define DEFAULT __attribute__((weak))

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)


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

#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || (defined EMULATOR_STM32F405RGT6)
typedef IRQn_Type cmnIRQn_t;
#endif

typedef uint16_t cmnEventBitmap_t;
#define CMN_EVENT_TIM2                (1<<0)
#define CMN_EVENT_TIM9                (1<<1)
#define CMN_EVENT_SCREEN_REFRESH_CPLT (1<<2)
#define CMN_EVENT_USER_KEY_M          (1<<3)  /*!< Use click the power button */
#define CMN_EVENT_USER_KEY_L          (1<<4)  /*!< Use click the power button */
#define CMN_EVENT_USER_KEY_R          (1<<5)  /*!< Use click the power button */
#define CMN_EVENT_SCREEN_DISPOFF      (1<<6)  /*!< Screen has been inactive for a while or turn off the display */
#define CMN_EVENT_SCREEN_DISPON       (1<<7)  /*!< Screen activated or turn on the display */
#define CMN_EVENT_SCREEN_DISPBR       (1<<8)  /*!< Screen brightness setting */
#define CMN_EVENT_QMI8658_TX_CPLT     (1<<9)  /*!< QMI8658 DMA TX completed on I2C1 */
#define CMN_EVENT_QMI8658_RX_CPLT     (1<<10) /*!< QMI8658 DMA RX completed on I2C1 */
#define CMN_EVENT_QMI8658_INT1        (1<<11) /*!< CTRL9 Command Done */
#define CMN_EVENT_QMI8658_INT2        (1<<12) /*!< FIFO Watermark Reached */
#define CMN_EVENT_UPDATE_RTC          (1<<13) /*!< System update RTC time */
#define CMN_EVENT_UART_INPUT          (1<<14) /*!< Received a new message from uart port */
#define CMN_EVENT_SYSTEM_INIT         (1<<(configMAX_NUM_OF_EVENT_GROUP_BITS-1)) /*!< System reboot/reset/boot completed */

#define CMN_DATE_YEAR_OFFSET          2022
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

typedef enum cmnWeekday_t{
  kWeekDay_Monday    = 0,
  kWeekDay_Tuesday   = 1,
  kWeekDay_Wednesday = 2,
  kWeekDay_Thursday  = 3,
  kWeekDay_Friday    = 4,
  kWeekDay_Saturday  = 5,
  kWeekDay_Sunday    = 6,
  kWeekDay_TOTAL
}cmnWeekday_t;


#ifdef __cplusplus
}
#endif  // extern C
#endif  // CMN_TEST_H


/**
 * @ref https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
 */
#if defined(__GNUC__) || defined(__GNUG__)
  #pragma GCC diagnostic pop
#elif defined(__clang__)
  #pragma clang diagnostic pop
#elif defined(__ICC_ARM__)
  
#elif defined(_MSC_VER)

#endif
