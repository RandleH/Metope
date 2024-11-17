/**
 ******************************************************************************
 * @file    cmn_interrupt.h
 * @author  RandleH
 * @brief   Common Program - Interrupt Service Routine
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


#include <stdint.h>
#include "cmn_type.h"


#ifndef CMN_INTERRUPT_H
#define CMN_INTERRUPT_H

#ifdef __cplusplus
extern "C"{
#endif

/* ************************************************************************** */
/*                      Interrupt Management Structure                        */
/* ************************************************************************** */
#define NUM_PENDING_INTERRUPT       10

typedef struct{
  cmnBoolean_t  operation;
  cmnIRQn_t     irqn;
  uint32_t      target_cnt;
}tCmnInterruptUnit;

typedef struct{
  tCmnInterruptUnit record[NUM_PENDING_INTERRUPT];
  uint8_t           record_cnt;
}tCmnInterruptProc;
extern tCmnInterruptProc gInterruptProcess;



void cmn_interrupt_disable_irq_after( const cmnIRQn_t irq, const uint32_t ms);
void cmn_interrupt_enable_irq_after( const cmnIRQn_t irq, const uint32_t ms);
void cmn_interrupt_init_priority( void);

#ifdef __cplusplus
}
#endif


#endif
