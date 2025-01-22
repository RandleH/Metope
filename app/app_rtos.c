/**
 ******************************************************************************
 * @file    app_rtos.c
 * @author  RandleH
 * @brief   Application - FreeRTOS Application Interface
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


/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include "FreeRTOS.h"
#include "global.h"
#include "trace.h"
#include "assert.h"
#include "app_rtos.h"
#include "app_clock.h"
#include "cmn_type.h"


#ifdef __cplusplus
extern "C"{
#endif

void app_rtos_start(void){
  metope.app.rtos.status.running = true;
  vTaskStartScheduler();
}

void app_rtos_stop(void){
  metope.app.rtos.status.running = false;
  vTaskEndScheduler();
}

/**
 * @brief Application Idle Thread
 * @note All function marked `RTOSIDLE` within the app domain will be called here
 */
void app_rtos_idle_callback(void) {
  if( 0!=metope.app.rtos.task.bitmap_idle.clock ){
    app_clock_idle(&metope.app.clock);
    metope.app.rtos.task.bitmap_idle.clock = 0;
  }

}



/**
 * @ref
 *  https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/12-Hook-functions
 * 
 * @note
 *  The idle task can optionally call an application defined hook (or callback) function - the idle hook. 
 *  The idle task runs at the very lowest priority, so such an idle hook function will only get executed
 *  when there are no tasks of higher priority that are able to run. This makes the idle hook function 
 *  an ideal place to put the processor into a low power state - providing an automatic power saving 
 *  whenever there is no processing to be performed.
 * 
 *  The idle hook will only get called if configUSE_IDLE_HOOK is set to 1 within FreeRTOSConfig.h.
 * 
 *  The idle hook is called repeatedly as long as the idle task is running. It is paramount that the idle 
 *  hook function does not call any API functions that could cause it to block. Also, if the application 
 *  makes use of the vTaskDelete() API function then the idle task hook must be allowed to periodically 
 *  return (this is because the idle task is responsible for cleaning up the resources that were allocated
 *  by the RTOS kernel to the task that has been deleted).
 * 
 * @addtogroup FreeRTOS
 */
void vApplicationIdleHook(void){
  app_rtos_idle_callback();
}


/**
 * @addtogroup FreeRTOS
 */
void vApplicationMallocFailedHook (void){
  ASSERT(0, "FreeRTOS malloc failed");
}

/**
 * @addtogroup FreeRTOS
 */
void vApplicationGetIdleTaskMemory (StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
  /* Idle task control block and stack */
  static StaticTask_t Idle_TCB;
  static StackType_t  Idle_Stack[configIDLE_TASK_STACK_SIZE];

  *ppxIdleTaskTCBBuffer   = &Idle_TCB;
  *ppxIdleTaskStackBuffer = &Idle_Stack[0];
  *pulIdleTaskStackSize   = (uint32_t)configIDLE_TASK_STACK_SIZE;
}

/**
 * @addtogroup FreeRTOS
 */
void vApplicationStackOverflowHook (TaskHandle_t xTask, signed char *pcTaskName) {
  (void)xTask;
  ASSERT(0, "Stack overflow detected. Task=%s", pcTaskName);
}

/**
 * @addtogroup FreeRTOS
 */
void vApplicationGetTimerTaskMemory (StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
  /* Timer task control block and stack */
  static StaticTask_t Timer_TCB;
  static StackType_t  Timer_Stack[configTIMER_TASK_STACK_DEPTH];

  *ppxTimerTaskTCBBuffer   = &Timer_TCB;
  *ppxTimerTaskStackBuffer = &Timer_Stack[0];
  *pulTimerTaskStackSize   = (uint32_t)configTIMER_TASK_STACK_DEPTH;
}






#ifdef __cplusplus
}
#endif