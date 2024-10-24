
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "bsp_led.h"
#include "cmn_device.h"



void inline bsp_led__on( void){
  
}



void inline bsp_led__off( void){

}

void inline bsp_led__toggle( void){
  LED_GPIO_Port->ODR ^= LED_Pin;
}

void inline bsp_led__switch( cmnBoolean_t status){
  if(ON==status){
    LED_GPIO_Port->BSRR = (uint32_t)(LED_Pin << 16U);
  }
  else{
    LED_GPIO_Port->BSRR = LED_Pin;
  }
}