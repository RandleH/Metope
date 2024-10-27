



#ifndef CMN_DEVICE_H
#define CMN_DEVICE_H


#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define KEY_R_Pin GPIO_PIN_1
#define KEY_R_GPIO_Port GPIOA
#define KEY_R_EXTI_IRQn EXTI1_IRQn
#define DBG_TX_Pin GPIO_PIN_2
#define DBG_TX_GPIO_Port GPIOA
#define DBG_RX_Pin GPIO_PIN_3
#define DBG_RX_GPIO_Port GPIOA
#define SCREEN_RST_Pin GPIO_PIN_2
#define SCREEN_RST_GPIO_Port GPIOB
#define SCREEN_CS_Pin GPIO_PIN_12
#define SCREEN_CS_GPIO_Port GPIOB
#define TP_INT_Pin GPIO_PIN_9
#define TP_INT_GPIO_Port GPIOA
#define TP_INT_EXTI_IRQn EXTI9_5_IRQn
#define KEY_L_Pin GPIO_PIN_15
#define KEY_L_GPIO_Port GPIOA
#define KEY_L_EXTI_IRQn EXTI15_10_IRQn

#endif



#ifdef __cplusplus
extern "C"{
#endif


#ifdef __cplusplus
}
#endif

#endif
