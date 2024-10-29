

#ifndef GLOBAL_H
#define GLOBAL_H

#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
#include "stm32f4xx_hal.h"
#endif

#ifdef __cplusplus
extern "C"{
#endif


extern SPI_HandleTypeDef   hspi2;
extern DMA_HandleTypeDef   hdma_spi2_tx;
extern TIM_HandleTypeDef   htim3;
extern UART_HandleTypeDef  huart2;


#ifdef __cplusplus
}
#endif

#if (defined TEST_ONLY) && (defined __cplusplus)

#include "test.hh"

extern LocalProjectTest     tb_infra_local;
extern HumanInteractionTest tb_infra_hmi;

#endif
#endif