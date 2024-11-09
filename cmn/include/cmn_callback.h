

#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  #include "stm32f4xx_hal.h"
  #include "stm32f4xx_hal_spi.h"
#endif


#ifndef CMN_CALLBACK_H
#define  CMN_CALLBACK_H



#ifdef __cplusplus
extern "C"{
#endif

void cmn_callback_screen_spi_completed(SPI_HandleTypeDef *hspi);


#ifdef __cplusplus
}
#endif






#endif
