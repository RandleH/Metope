





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
  if (p_rtos->status.running) {
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
  if (p_rtos->status.running) {
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
  if (p_rtos->status.running) {
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