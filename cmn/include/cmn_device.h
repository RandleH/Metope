/**
 ******************************************************************************
 * @file    cmn_device.h
 * @author  RandleH
 * @brief   Common Program - Device I/O Port Declaration
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



#ifndef CMN_DEVICE_H
#define CMN_DEVICE_H


#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#define LED_Pin               GPIO_PIN_13
#define LED_GPIO_Port         GPIOC
#define KEY_R_Pin             GPIO_PIN_1
#define KEY_R_GPIO_Port       GPIOA
#define KEY_R_EXTI_IRQn       EXTI1_IRQn
#define DBG_TX_Pin            GPIO_PIN_2
#define DBG_TX_GPIO_Port      GPIOA
#define DBG_RX_Pin            GPIO_PIN_3
#define DBG_RX_GPIO_Port      GPIOA
#define FLASH_CS_Pin          GPIO_PIN_4
#define FLASH_CS_GPIO_Port    GPIOA
#define FLASH_CLK_Pin         GPIO_PIN_5
#define FLASH_CLK_GPIO_Port   GPIOA
#define FLASH_MOSI_Pin        GPIO_PIN_7
#define FLASH_MOSI_GPIO_Port  GPIOA
#define SCREEN_DC_Pin         GPIO_PIN_2
#define SCREEN_DC_GPIO_Port   GPIOB
#define SCREEN_CLK_Pin        GPIO_PIN_13
#define SCREEN_CLK_GPIO_Port  GPIOB
#define SCREEN_MISO_Pin       GPIO_PIN_14
#define SCREEN_MISO_GPIO_Port GPIOB
#define SCREEN_MOSI_Pin       GPIO_PIN_15
#define SCREEN_MOSI_GPIO_Port GPIOB
#define TP_INT_Pin            GPIO_PIN_9
#define TP_INT_GPIO_Port      GPIOA
#define TP_INT_EXTI_IRQn      EXTI9_5_IRQn
#define KEY_L_Pin             GPIO_PIN_15
#define KEY_L_GPIO_Port       GPIOA
#define KEY_L_EXTI_IRQn       EXTI15_10_IRQn
#define KEY_M_Pin             GPIO_PIN_0
#define KEY_M_GPIO_Port       GPIOA
#define KEY_M_EXTI_IRQn       EXTI0_IRQn
#define FLASH_MISO_Pin        GPIO_PIN_4
#define FLASH_MISO_GPIO_Port  GPIOB
#define GYRO_INT1_Pin         GPIO_PIN_5
#define GYRO_INT1_GPIO_Port   GPIOB
#define GYRO_INT1_EXTI_IRQn   EXTI9_5_IRQn
#define GYRO_INT2_Pin         GPIO_PIN_6
#define GYRO_INT2_GPIO_Port   GPIOB
#define GYRO_INT2_EXTI_IRQn   EXTI9_5_IRQn
#define GYRO_EN_Pin           GPIO_PIN_7
#define GYRO_EN_GPIO_Port     GPIOB
#define GYRO_SCK_Pin          GPIO_PIN_8
#define GYRO_SCK_GPIO_Port    GPIOB
#define GYRO_SDA_Pin          GPIO_PIN_9
#define GYRO_SDA_GPIO_Port    GPIOB

#endif



#ifdef __cplusplus
extern "C"{
#endif


#ifdef __cplusplus
}
#endif

#endif
