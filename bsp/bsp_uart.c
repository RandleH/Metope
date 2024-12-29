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

/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "global.h"
#include "cmn_type.h"
#include "cmn_utility.h"


/* ************************************************************************** */
/*                               Private Macros                               */
/* ************************************************************************** */
#define BSP_UART_PRINTF_BUF_SIZE        64

#define FMT_STR_UNSET                    0
#define FMT_STR_HEX                      1
#define FMT_STR_DEC_SIGNED               2
#define FMT_STR_DEC_UNSIGNED             3
#define FMT_STR_STR                      4


#ifdef __cplusplus
extern "C"{
#endif


/**
 * @brief
 * @param [in] format - Formatted String
 * @param [in] ...    - Must be `uint32_t` / `int32_t`
 * @note  Only support `%u` | `%x` | `%d` with width indicator
 * @note  Automatically end with a new line
 * @return `SUCCESS` | `ERROR` - Something wrong with prarmeters
 */
int bsp_uart_printf( const char *format, ...){
  va_list va;
  va_start(va, format);
    
  char    buf[BSP_UART_PRINTF_BUF_SIZE] = {0};
  uint8_t idx     = 0;

  while(*format && idx<sizeof(buf)) {
    if(*format != '%') {
        buf[idx++] = *format++;
        continue;
    }
    else {
        ++format;
    }
    char c = *format++;

    uint8_t flag  = FMT_STR_UNSET;
    uint8_t width = 0xFF;

    while(flag==FMT_STR_UNSET){
      switch(c) {
        case 'u':
        case 'U':{
          if(width==0xFF){
            idx += cmn_utility_uint2strdec( &buf[idx], sizeof(buf)-idx, va_arg(va, uint32_t));
          }else{
            idx += cmn_utility_uint2strdec_width( &buf[idx], sizeof(buf)-idx, va_arg(va, uint32_t), width);
          }
          flag = FMT_STR_DEC_UNSIGNED;
          break;
        }
        case 'd':
        case 'D':{
          if(width==0xFF){
            idx += cmn_utility_int2strdec( &buf[idx], sizeof(buf)-idx, va_arg(va, int32_t));
          }else{
            idx += cmn_utility_int2strdec_width( &buf[idx], sizeof(buf)-idx, va_arg(va, int32_t), width);
          }
          flag = FMT_STR_DEC_SIGNED;
          break;
        }
        case 'x':
        case 'X':{
          if(width==0xFF){
            idx += cmn_utility_uint2strhex( &buf[idx], sizeof(buf)-idx, va_arg(va, uint32_t));
          }else{
            idx += cmn_utility_uint2strhex_width( &buf[idx], sizeof(buf)-idx, va_arg(va, uint32_t), width);
          }
          flag = FMT_STR_HEX;
          break;
        }
        case 's':
        case 'S':{
          const char *str = va_arg(va, char*);
          size_t      len = strlen(str);
          if( len > sizeof(buf)-idx -1 ){
            memset(&buf[idx], '#', sizeof(buf)-idx-1);
            idx = sizeof(buf)-1;
          }else{
            strncpy(&buf[idx], str, sizeof(buf)-idx);
            idx += len;
          }
          flag = FMT_STR_STR;
          break;
        }
        case '0':{
          c = *format++;
          break;
        }
        case '1'...'9':{
          width = c - '0';
          c = *format++;
          break;
        }
        default:{
#if 1
          const char *msg = "Unable to print the message => ";
          HAL_UART_Transmit( metope.dev.pHuart2, msg, strlen(msg), HAL_MAX_DELAY);
          HAL_UART_Transmit( metope.dev.pHuart2, format, strlen(format), HAL_MAX_DELAY);
#else
          printf("Unable to print the message => %s\n", format);
#endif
          return ERROR;
        }
      }
    }

  }

  va_end(va);
  idx = CMN_MIN( idx, sizeof(buf)-1);
  buf[idx++] = '\0';

#if 1
  HAL_StatusTypeDef hstatus = HAL_UART_Transmit( metope.dev.pHuart2, buf, idx, HAL_MAX_DELAY);

  return hstatus==HAL_OK?SUCCESS : ERROR;
#else
  // printf("%s", buf);
  std::cout << buf << std::endl;
  return SUCCESS;
#endif

}

#ifdef __cplusplus
}
#endif