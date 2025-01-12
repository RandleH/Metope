/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 * https://www.freertos.org/Documentation/02-Kernel/03-Supported-devices/02-Customization
 *
 * 1 tab == 2 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H


/* Ensure stdint is only used by the compiler, and not the assembler. */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
 #include <stdint.h>
 extern uint32_t SystemCoreClock;
#endif

/*  CMSIS-RTOSv2 defines 56 levels of priorities. To be able to use them
 *  all and avoid application misbehavior, configUSE_PORT_OPTIMISED_TASK_SELECTION
 *  must be set to 0 and configMAX_PRIORITIES to 56
 *
 */

/* ************************************************************************** */
/*                               Constant Macro                               */
/* ************************************************************************** */
#define configTICK_RATE_HZ                   ((TickType_t)1000)
#define configMINIMAL_STACK_SIZE             ((uint16_t)128)
#define configMAX_PRIORITIES					       (56U)
#define configMAX_TASK_NAME_LEN              (8)

/**
 * @ref
 *  https://www.freertos.org/Documentation/02-Kernel/04-API-references/12-Event-groups-or-flags/06-xEventGroupSetBitsFromISR
 * @note
 *  As mentioned in the paragraph above, setting bits from an ISR will defer the 
 *  set operation to the RTOS daemon task (also known as the timer service task). 
 *  The RTOS daemon task is scheduled according to its priority, 
 *  just like any other RTOS task. 
 *  Therefore, if it is essential the set operation completes immediately 
 *  (before a task created by the application executes) then the priority of 
 *  the RTOS daemon task must be higher than the priority of any application task 
 *  that uses the event group. The priority of the RTOS daemon task is set by 
 *  the configTIMER_TASK_PRIORITY definition in FreeRTOSConfig.h.
 */
#define configTIMER_TASK_PRIORITY            configMAX_PRIORITIES

#define configQUEUE_REGISTRY_SIZE            (8)
#define configNUMBER_OF_CORES                (1)
#define configTOTAL_HEAP_SIZE                ((size_t)(10 * 1024))

/* ************************************************************************** */
/*                               Property Macro                               */
/* ************************************************************************** */
#define USE_FreeRTOS_HEAP_4
#define configUSE_PREEMPTION                 1
#define configUSE_TIME_SLICING               1
#define configUSE_IDLE_HOOK                  1
#define configUSE_TICK_HOOK                  0
#define configUSE_16_BIT_TICKS               0
#define configUSE_MUTEXES                    1
#define configUSE_TIMERS                     1
#if defined (configUSE_TIMERS) && (configUSE_TIMERS==1)
  #define configTIMER_QUEUE_LENGTH             10
  #define configTIMER_TASK_STACK_DEPTH         1024
#endif
#define configUSE_RECURSIVE_MUTEXES          1
#define configUSE_MALLOC_FAILED_HOOK         1
#define configUSE_APPLICATION_TASK_TAG       0
#define configUSE_COUNTING_SEMAPHORES        1
#define configUSE_CO_ROUTINES                0
#if defined (configUSE_CO_ROUTINES) && (configUSE_CO_ROUTINES==1)
  #define configMAX_CO_ROUTINE_PRIORITIES      2
#endif
#define configSUPPORT_STATIC_ALLOCATION      1
#define configSUPPORT_DYNAMIC_ALLOCATION     1
#define configIDLE_SHOULD_YIELD              1
#define INCLUDE_vTaskPrioritySet             1
#define INCLUDE_uxTaskPriorityGet            1
#define INCLUDE_vTaskDelete                  1
#define INCLUDE_vTaskCleanUpResources        0
#define INCLUDE_vTaskSuspend                 1
#define INCLUDE_vTaskDelayUntil              1
#define INCLUDE_vTaskDelay                   1
#define INCLUDE_xTaskGetSchedulerState       1
#define INCLUDE_xTimerPendFunctionCall       1
#define INCLUDE_xEventGroupSetBitFromISR     1
#define INCLUDE_xTaskGetHandle               1
#define INCLUDE_uxTaskGetStackHighWaterMark  1
#define INCLUDE_uxTaskGetStackHighWaterMark2 1
#define INCLUDE_xSemaphoreGetMutexHolder     1
#define INCLUDE_xTaskGetCurrentTaskHandle    1
#define INCLUDE_eTaskGetState                1


/* ************************************************************************** */
/*                            Function Like Macro                             */
/* ************************************************************************** */
#include "assert.h"
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); ASSERT(0, "FreeRTOS Assertion"); }

/**
 * @note
 *  Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
 *  standard names.
 * @attention
 *  FreeRTOS is using the SysTick as internal time base, thus make sure the system and peripherials are 
 *  using a different time base (TIM based for example).
 */
 #define vPortSVCHandler      FreeRTOS_SVC_Handler
 #define xPortPendSVHandler   FreeRTOS_FreeRTOS_PendSV_Handler
 #define xPortSysTickHandler  FreeRTOS_SysTick_Handler





/* ************************************************************************** */
/*                              Customized Macro                              */
/* ************************************************************************** */
#define configIDLE_TASK_STACK_SIZE     (1024U)


/* ************************************************************************** */
/*                               Selective Macro                              */
/* ************************************************************************** */
#ifdef DEBUG
  #define configRECORD_STACK_HIGH_ADDRESS         1
  #define configUSE_TRACE_FACILITY                1
  #define configGENERATE_RUN_TIME_STATS           0
  #define configUSE_PORT_OPTIMISED_TASK_SELECTION	0
  #define INCLUDE_pcGetTaskName                   1
  #define configCHECK_FOR_STACK_OVERFLOW          1
#else
  #define configGENERATE_RUN_TIME_STATS           0
  #define configUSE_PORT_OPTIMISED_TASK_SELECTION	0
  #define INCLUDE_pcGetTaskName                   0
  #define configCHECK_FOR_STACK_OVERFLOW          1
#endif

#if (defined SYS_TARGET_STM32F411CEU6) 
  #define configCPU_CLOCK_HZ                (96000000U)
#elif defined (SYS_TARGET_STM32F405RGT6)
  #define configCPU_CLOCK_HZ                (84000000U)
#endif




/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
 /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
 #define configPRIO_BITS         __NVIC_PRIO_BITS
#else
 #define configPRIO_BITS         4        /* 15 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY   0xf

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY   ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY  ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )


#endif /* FREERTOS_CONFIG_H */

