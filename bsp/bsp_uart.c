/**
 ******************************************************************************
 * @file    bsp_uart.c
 * @author  RandleH
 * @brief   Global Variable Declaration
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 RandleH.
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
#include <stdarg.h>
#include <string.h>
#include "global.h"
#include "cmn_type.h"
#include "cmn_utility.h"





#ifdef __cplusplus
extern "C"{
#endif



/**
 * @brief
 * @param [in] format - Formatted String
 * @param [in] ...    - Must be uint32_t
 * @note  Only support `%u` | `%x` | `%d` with width indicator
 * @note  Automatically end with a new line
 * @return `SUCCESS` | `ERROR` - Something wrong with prarmeters
 */
int bsp_uart_printf( const char *format, ...){
  va_list va;
  va_start(va, format);
    
  char    buf[32] = {0};
  uint8_t idx     = 0;

  while(*format && idx<sizeof(buf)) {
    if(*format != '%') {
        buf[idx++] = *format++;
        continue;
    }
    else {
        ++format;
    }
    char c = *format;

bsp_uart_printf_SELECT_FLAG:
    switch(c) {
      case 'u':{
        cmn_utility_uint2strdec( &buf[idx], sizeof(buf)-idx, va_arg(va, uint32_t));
        break;
      }
      case 'd':{
        cmn_utility_int2strdec( &buf[idx], sizeof(buf)-idx, va_arg(va, int32_t));
        break;
      }
      case 'x':{
        cmn_utility_uint2strhex( &buf[idx], sizeof(buf)-idx, va_arg(va, int32_t));
        break;
      }
      case '0':{
        c = *++format;
        goto bsp_uart_printf_SELECT_FLAG;
      }
      default:{
        const char *msg = "Unable to print the message";
        HAL_UART_Transmit( metope.dev.pHuart2, msg, strlen(msg), HAL_MAX_DELAY);
        return ERROR;
      }
    }
  }

  va_end(va);

  hstatus = HAL_UART_Transmit( metope.dev.pHuart2, buf, idx, HAL_MAX_DELAY);

  return hstatus==HAL_OK ? SUCCESS : ERROR;
}





#ifdef __cplusplus
}
#endif