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


#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "bsp_led.h"
#include "cmn_device.h"


/**
 * @todo
 * @addtogroup MachineDependent
 */
void inline bsp_led__on( void){
  
}


/**
 * @todo
 * @addtogroup MachineDependent
 */
void inline bsp_led__off( void){

}

/**
 * @addtogroup MachineDependent
 */
void inline bsp_led__toggle( void){
  LED_GPIO_Port->ODR ^= LED_Pin;
}

/**
 * @addtogroup MachineDependent
 */
void inline bsp_led__switch( cmnBoolean_t status){
  if(ON==status){
    LED_GPIO_Port->BSRR = (uint32_t)(LED_Pin << 16U);
  }
  else{
    LED_GPIO_Port->BSRR = LED_Pin;
  }
}