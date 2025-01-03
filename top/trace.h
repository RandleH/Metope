/**
 ******************************************************************************
 * @file    trace.h
 * @author  RandleH
 * @brief   Project trace message
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

#ifndef TRACE_H
#define TRACE_H



#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  #include "bsp_uart.h"
  #define TRACE_PRINTF( fmt, ...) bsp_uart_printf( fmt, ##__VA_ARGS__)
#elif (defined SYS_TARGET_NATIVE)
  #include <stdio.h>
  #define TRACE_PRINTF( fmt, ...) printf( fmt, ##__VA_ARGS__)
#else
  
#endif





/**
 * @ref: https://www.theurbanpenguin.com/4184-2/
 */
#define FMT_BLUE         "\033[0;34m"
#define FMT_RED          "\033[0;31m"
#define FMT_GREEN        "\x1b[32;20m"
#define FMT_B_RED        "\033[31;1m"
#define FMT_YELLOW       "\033[33;20m"
#define FMT_RESET        "\033[0m"
#define FMT_ITALIC       "\x1b[3m"
#define FMT_ITALIC_RESET "\x1b[23m"

#define FMT_DEBUG_STR    FMT_BLUE   "  DEBUG: "
#define FMT_INFO_STR                "   INFO: "
#define FMT_WARN_STR     FMT_YELLOW "WARNING: "
#define FMT_ERROR_STR    FMT_B_RED  "  ERROR: "

#if defined(LOG_LEVEL) && (LOG_LEVEL==2)
  #define TRACE_DEBUG( fmt, ...) TRACE_PRINTF( "%s" fmt FMT_RESET, FMT_DEBUG_STR, ##__VA_ARGS__)
#else
  #define TRACE_DEBUG( fmt, ...)
#endif

#if defined(LOG_LEVEL) && (LOG_LEVEL>=1)
  #define TRACE_INFO( fmt, ...) TRACE_PRINTF( "%s" fmt, FMT_INFO_STR, ##__VA_ARGS__)
#else
  #define TRACE_INFO( fmt, ...)
#endif

#if defined(LOG_LEVEL) && (LOG_LEVEL>=0)
  #define TRACE_WARNING( fmt, ...) TRACE_PRINTF( "%s" fmt FMT_RESET, FMT_WARN_STR, ##__VA_ARGS__)
  #define TRACE_ERROR( fmt, ...)   TRACE_PRINTF( "%s" fmt FMT_RESET, FMT_ERROR_STR, ##__VA_ARGS__)
#else
  #define TRACE_WARNING( fmt, ...)
  #define TRACE_ERROR( fmt, ...)
#endif

#endif



/* ********************************** EOF *********************************** */
