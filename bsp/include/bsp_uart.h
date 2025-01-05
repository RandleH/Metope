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

#ifdef __cplusplus
extern "C"{
#endif


int bsp_uart_printf( const char *format, ...); // __attribute__ (( format(printf,1,2)));


#ifdef __cplusplus
}
#endif



#endif