


#ifndef DEVICE_H
#define DEVICE_H

#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  #include "stm32f4xx.h"
#else
  #error "Unknown Device Header"
#endif




#endif