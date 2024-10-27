





#include "cmn_type.h"


#ifndef BSP_LED_H
#define BSP_LED_H

#ifdef __cplusplus
extern "C"{
#endif

void bsp_led__on( void);
void bsp_led__off( void);
void bsp_led__toggle( void);
void bsp_led__switch( cmnBoolean_t status);

#ifdef __cplusplus
}
#endif


#endif




