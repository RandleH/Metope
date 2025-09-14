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
#define BSP_UART_PRINTF_BUF_SIZE         256


/* ************************************************************************** */
/*                              Public Functions                              */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

void bsp_uart_init(void) {
  tBspUart *p_uart = &metope.bsp.uart;

#ifndef UNIT_TEST
  /* Use RX interrupt for main program */
  USART2->CR1 |= USART_CR1_RXNEIE;
  USART2->CR3 |= USART_CR3_EIE;
#endif

  p_uart->rx_buf[BSP_CFG_UART_RX_BUF_SIZE] = '\0';
  p_uart->rx_status.word                   = 0;
  p_uart->rx_idx                           = 0;
}

/**
 * @brief
 * @param [in] format - Formatted String
 * @param [in] ...    - Must be `uint32_t` / `int32_t`
 * @note  Only support `%u` | `%x` | `%d` with width indicator
 * @note  Automatically end with a new line
 * @return `SUCCESS` | `ERROR` - Something wrong with prarmeters
 */
int bsp_uart_printf( const char *format, ...){
  tBspUart *p_uart = &metope.bsp.uart;
  va_list va;
  va_start(va, format);
  int num_c_inserted = cmn_utility_vsnprintf( p_uart->tx_buf, BSP_CFG_UART_TX_BUF_SIZE, format, va);
  va_end(va);

  p_uart->tx_buf[BSP_CFG_UART_TX_BUF_SIZE] = '\0';

  if(num_c_inserted==0){
    const char *msg = "Unable to print the message => ";
    HAL_UART_Transmit( metope.dev.pHuart2, msg, strlen(msg), HAL_MAX_DELAY);
    HAL_UART_Transmit( metope.dev.pHuart2, format, strlen(format), HAL_MAX_DELAY);
    return ERROR;
  }

#if 1
  HAL_StatusTypeDef hstatus;
  hstatus  = HAL_UART_Transmit( metope.dev.pHuart2, p_uart->tx_buf, num_c_inserted, HAL_MAX_DELAY);
  p_uart->tx_buf[0] = '\n';
  p_uart->tx_buf[1] = '\0';
  hstatus |= HAL_UART_Transmit( metope.dev.pHuart2, p_uart->tx_buf, 2, HAL_MAX_DELAY);
  return hstatus==HAL_OK ? SUCCESS : ERROR;
#else
#ifdef __cplusplus
  std::cout << buf << std::endl;
#else
  printf("%s\n", buf);
#endif
  return SUCCESS;
#endif

}

#ifdef __cplusplus
}
#endif

/* ********************************** EOF *********************************** */
