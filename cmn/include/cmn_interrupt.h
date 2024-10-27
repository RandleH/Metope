


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


#ifdef __cplusplus
}
#endif


#endif
