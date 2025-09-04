/**
 ******************************************************************************
 * @file    assert.h
 * @author  RandleH
 * @brief   Project Assertion
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


#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
#include "bsp_uart.h"
#include "bsp_led.h"
#elif defined (SYS_TARGET_NATIVE)
#include <stdlib.h> 
#endif

#ifndef ASSERT_H
#define ASSERT_H


#ifdef DEBUG
  #if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
    #define ASSERT( expr, msg, ...)                               \
      do{                                                         \
        if(!(expr)){                                              \
          bsp_uart_printf("Assertion@%s:%u", __FILE__, __LINE__); \
          bsp_uart_printf( (msg),  ##__VA_ARGS__);                \
          bsp_led_on();                                           \
          __BKPT(0);                                              \
          bsp_led_off();                                          \
        }                                                         \
      }while(0)
    #elif defined (SYS_TARGET_NATIVE)
      #define ASSERT( expr, msg, ...)                             \
        do{                                                       \
          if(!((expr) && (msg))) {                                \
            printf(msg);                                          \
            exit(1);                                              \
          }                                                       \
        }while(0)
    #endif
#else
  #define ASSERT( expr, msg)
#endif









#endif
