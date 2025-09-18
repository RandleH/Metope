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
#include "cmn_delay.h"


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
    HAL_UART_Transmit( metope.bsp.pHuart2, msg, strlen(msg), HAL_MAX_DELAY);
    HAL_UART_Transmit( metope.bsp.pHuart2, format, strlen(format), HAL_MAX_DELAY);
    return ERROR;
  }

#if 1
  int ret = SUCCESS;
  {
    uint32_t tmpreg = 0x00U;
    tmpreg = USART2->CR1;
    tmpreg &= ~(USART_CR1_TCIE | USART_CR1_TXEIE);
    tmpreg |= USART_CR1_TE;
    USART2->CR1 = tmpreg;
    while( 0 == READ_BIT(USART2->CR1, USART_CR1_TE));
  }

  for( int i = 0; i < num_c_inserted; ++i) {
    p_uart->tx_status.escape_ticks = 0;

    /* Wait until DR can accept new data */
    while( (0 == (USART2->SR & USART_SR_TXE)) && (p_uart->tx_status.escape_ticks != 0xFF) ) {
      // if(metope.rtos.status.running) {
      //   vTaskDelay(1);
      // }else{
      //   cmnBoolean_t async_mode = false;
      //   cmn_tim9_sleep(30000, async_mode);
      // }
      // p_uart->tx_status.escape_ticks++;
    }

    USART2->DR = (uint8_t)(p_uart->tx_buf[i] & (uint8_t)0xFF);
    
    if (p_uart->tx_status.escape_ticks == 0xFF) {
      ret = ERROR;
    }
  }

  p_uart->tx_buf[0] = '\n';
  p_uart->tx_buf[1] = '\0';

  for( int i = 0; i < 2; ++i) {
    p_uart->tx_status.escape_ticks = 0;

    /**
     * the TXE (Transmit Empty) flag indicates that the Data Register (DR) can accept a new byte of data, allowing you to load the next byte into the transmit holding register. The TC (Transmission Complete) flag indicates that the entire last transmission, including the stop bit, has been fully sent out of the shift register and onto the TX line. 
     */
    while( (0 == (USART2->SR & USART_SR_TXE)) && (p_uart->tx_status.escape_ticks != 0xFF) ) {
      // if(metope.rtos.status.running) {
      //   vTaskDelay(1);
      // }else{
      //   cmnBoolean_t async_mode = false;
      //   cmn_tim9_sleep(3000, async_mode);
      // }
      // p_uart->tx_status.escape_ticks++;
    }

    USART2->DR = (uint8_t)(p_uart->tx_buf[i] & (uint8_t)0xFF);
    
    if (p_uart->tx_status.escape_ticks == 0xFF) {
      ret = ERROR;
    }
  }

  while((0 == (USART2->SR & USART_SR_TC))) {
    /* Wait until the datafram was completed */
  }

  {
    USART2->SR  &= ~USART_SR_TC;
    uint32_t tmpreg = 0x00U;
    tmpreg = USART2->CR1;
    tmpreg &= ~USART_CR1_TE;
    USART2->CR1 = tmpreg;
    while( (0 == READ_BIT(USART2->CR1, USART_CR1_TE)) && (0 != READ_BIT(USART2->SR, USART_SR_TC)));
  }
  return ret;
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
