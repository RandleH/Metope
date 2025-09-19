/**
 ******************************************************************************
 * @file    memory.cc
 * @author  RandleH
 * @brief   Memory Interface Layer
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


#include <stdlib.h>
#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || (defined EMULATOR_STM32F405RGT6)
#include "global.h"
#elif (defined SYS_TARGET_NATIVE)
#endif


#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Malloc Wrapper Function
 * 
 */
void *MALLOC(size_t xWantedSize) {
#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || (defined EMULATOR_STM32F405RGT6)
  extern void *pvPortMalloc( size_t xWantedSize );
  const tRtos *p_rtos = &metope.rtos;
  if (p_rtos->status->running[0]) {
    return pvPortMalloc(xWantedSize);
  }else {
    return malloc(xWantedSize);
  }
#elif (defined SYS_TARGET_NATIVE)
  return malloc(xWantedSize);
#endif
}
  
void FREE(void *pv) {
#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || (defined EMULATOR_STM32F405RGT6)
  extern void vPortFree( void *pv );
  const tRtos *p_rtos = &metope.rtos;
  if (p_rtos->status->running[0]) {
    vPortFree(pv);
  }else {
    free(pv);
  }
#elif (defined SYS_TARGET_NATIVE)
  free(pv);
#endif
}

void *REALLOC(void *pv, size_t xWantedSize) {
#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || (defined EMULATOR_STM32F405RGT6)
  extern void *pvPortRealloc( void *pv, size_t xWantedSize );
  const tRtos *p_rtos = &metope.rtos;
  if (p_rtos->status->running[0]) {
    return pvPortRealloc(pv, xWantedSize);
  }else {
    return realloc(pv, xWantedSize);
  }
#elif (defined SYS_TARGET_NATIVE)
  return realloc(pv, xWantedSize);
#endif
}

#ifdef __cplusplus
}
#endif