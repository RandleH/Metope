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

#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  #include "stm32f4xx_hal.h"
  #include "stm32f4xx_hal_gpio.h"
#endif

#include "bsp_led.h"
#include "cmn_device.h"

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
void inline bsp_led__on( void){
  PIN_LED(0);
}


/**
 * @addtogroup MachineDependent
 */
void inline bsp_led__off( void){
  PIN_LED(1);
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
    LED_GPIO_Port->BSRR |= (uint32_t)(LED_Pin << 16U);
  }
  else{
    LED_GPIO_Port->BSRR |= LED_Pin;
  }
}