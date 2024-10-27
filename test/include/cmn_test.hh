


#include "test.hh"
#include "cmn_type.h"
#include "cmn_interrupt.h"


#ifndef CMN_TEST_HH
#define CMN_TEST_HH


/* ************************************************************************** */
/*                         Bindings - TestInterrupt                           */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

extern cmnBoolean_t cmn_interrupt_push( const tCmnInterruptUnit *unit);
extern void cmn_interrupt_pop( void);
extern uint32_t cmn_interrupt_top( void);

#ifdef __cplusplus
}
#endif

extern Test tb_infra;



void add_cmn_test(void);

#endif
