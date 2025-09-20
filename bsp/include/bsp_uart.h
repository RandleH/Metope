/**
 ******************************************************************************
 * @file    bsp_uart.h
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
#ifndef BSP_UART_H
#define BSP_UART_H

/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include "bsp_type.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef union stBspUartTxStatus {
  struct {
    uint16_t escape_ticks : 8;
    uint16_t msg_len      : 8;
  };
  uint16_t word;
} tBspUartTxStatus;

typedef union stBspUartRxStatus {
  struct {
    uint8_t is_locked     : 1;
    uint8_t is_overflowed : 1;
    uint8_t has_new_msg   : 1;
    uint8_t error_code    : 5;
  };
  uint8_t word;
} tBspUartRxStatus;

typedef struct stBspUart{
  char             tx_buf[BSP_CFG_UART_TX_BUF_SIZE+1];
  tBspUartTxStatus tx_status;
  char             rx_buf[BSP_CFG_UART_RX_BUF_SIZE+1];
  uint8_t          rx_idx;
  tBspUartRxStatus rx_status;
  //...//
} tBspUart;

void bsp_uart_init(void);
int bsp_uart_printf( const char *format, ...); // __attribute__ (( format(printf,1,2)));


#ifdef __cplusplus
}
#endif



#endif