/**
 ******************************************************************************
 * @file    bsp_led.c
 * @author  RandleH
 * @brief   Board Support Package Delivery - LED
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

/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include "device.h"
#include "bsp_led.h"

#define PIN_LED(x)\
  do{\
    if((x)==0){\
      ((LED_GPIO_Port)->BSRR) |= (u32)((LED_Pin)<<16);\
    }else{\
      ((LED_GPIO_Port)->BSRR) |= (LED_Pin);\
    }\
  }while(0)


/**
 * @addtogroup MachineDependent
 */
void inline bsp_led_on( void){
  PIN_LED(0);
}


/**
 * @addtogroup MachineDependent
 */
void inline bsp_led_off( void){
  PIN_LED(1);
}

/**
 * @addtogroup MachineDependent
 */
void inline bsp_led_toggle( void){
  LED_GPIO_Port->ODR ^= LED_Pin;
}

/**
 * @addtogroup MachineDependent
 */
void inline bsp_led__switch( cmnBoolean_t status){
  if(ON==status){
    LED_GPIO_Port->BSRR |= (uint32_t)(LED_Pin << 16U);
  }
  else{
    LED_GPIO_Port->BSRR |= LED_Pin;
  }
}