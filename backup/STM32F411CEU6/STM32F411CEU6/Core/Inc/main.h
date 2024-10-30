/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define KEY_R_Pin GPIO_PIN_1
#define KEY_R_GPIO_Port GPIOA
#define KEY_R_EXTI_IRQn EXTI1_IRQn
#define DBG_TX_Pin GPIO_PIN_2
#define DBG_TX_GPIO_Port GPIOA
#define DBG_RX_Pin GPIO_PIN_3
#define DBG_RX_GPIO_Port GPIOA
#define SCREEN_DC_Pin GPIO_PIN_2
#define SCREEN_DC_GPIO_Port GPIOB
#define SCREEN_CLK_Pin GPIO_PIN_13
#define SCREEN_CLK_GPIO_Port GPIOB
#define SCREEN_MISO_Pin GPIO_PIN_14
#define SCREEN_MISO_GPIO_Port GPIOB
#define SCREEN_MOSI_Pin GPIO_PIN_15
#define SCREEN_MOSI_GPIO_Port GPIOB
#define TP_INT_Pin GPIO_PIN_9
#define TP_INT_GPIO_Port GPIOA
#define TP_INT_EXTI_IRQn EXTI9_5_IRQn
#define KEY_L_Pin GPIO_PIN_15
#define KEY_L_GPIO_Port GPIOA
#define KEY_L_EXTI_IRQn EXTI15_10_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
