

#ifndef GLOBAL_H
#define GLOBAL_H

#include "bsp_type.h"
#include "cmn_type.h"
#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
#include "stm32f4xx_hal.h"
#endif

#ifdef __cplusplus
extern "C"{
#endif


typedef struct{
  struct{
    struct{
      bspScreenBrightness_t _brightness;
      //...//
    }screen;

    struct{
      volatile u32 *_port;
    }led;
  
  }bsp;

  struct{
    SPI_HandleTypeDef  * const pHspi2;
    UART_HandleTypeDef * const pHuart2;
    TIM_HandleTypeDef  * const pHtim3;
    //...//
  }dev;

  struct{
    u8 dummy;
  }app;

} tMainSystemStatus;


extern tMainSystemStatus   metope;

extern SPI_HandleTypeDef   hspi2;
extern DMA_HandleTypeDef   hdma_spi2_tx;
extern TIM_HandleTypeDef   htim3;
extern UART_HandleTypeDef  huart2;


#ifdef __cplusplus
}
#endif

#if (defined TEST_ONLY) && (TEST_ONLY==1) && (defined __cplusplus)

#include "test.hh"

extern LocalProjectTest     tb_infra_local;
extern HumanInteractionTest tb_infra_hmi;

#endif
#endif