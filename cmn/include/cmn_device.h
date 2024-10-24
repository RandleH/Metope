



#ifndef CMN_DEVICE_H
#define CMN_DEVICE_H


#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define KEY_D_Pin GPIO_PIN_0
#define KEY_D_GPIO_Port GPIOA
#define KEY_R_Pin GPIO_PIN_1
#define KEY_R_GPIO_Port GPIOA
#define KEY_R_EXTI_IRQn EXTI1_IRQn
#define SCREEN_RST_Pin GPIO_PIN_2
#define SCREEN_RST_GPIO_Port GPIOB
#define SCREEN_CS_Pin GPIO_PIN_12
#define SCREEN_CS_GPIO_Port GPIOB
#define KEY_L_Pin GPIO_PIN_15
#define KEY_L_GPIO_Port GPIOA
#define KEY_L_EXTI_IRQn EXTI15_10_IRQn

#endif


#endif
