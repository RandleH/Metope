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



#include "bsp_uart.h"
#include "bsp_led.h"

#ifndef ASSERT_H
#define ASSERT_H



#ifdef DEBUG
#ifdef __cplusplus
extern "C"{
#endif

#ifdef __cplusplus
}
#endif


#define ASSERT( expr, msg, ...)                               \
  do{                                                         \
    if(!(expr)){                                                \
      bsp_uart_printf("Assertion@%s:%u", __FILE__, __LINE__); \
      bsp_uart_printf( (msg),  ##__VA_ARGS__);                  \
      bsp_led_on();                                           \
      __BKPT(0);                                              \
      bsp_led_off();                                          \
    }                                                         \
  }while(0)
#else
#define ASSERT( expr, msg)
#endif









#endif