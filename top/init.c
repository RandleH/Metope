/**
 ******************************************************************************
 * @file    init.c
 * @author  RandleH
 * @brief   Initialization Program
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
#if (defined SYS_TARGET_STM32F411CEU6) || (defined EMULATOR_STM32F411CEU6)
  #include "STM32CubeMX/STM32F411CEU6/Core/Inc/main.h"
  #include "STM32CubeMX/STM32F411CEU6/Core/Src/main.c"
  #include "STM32CubeMX/STM32F411CEU6/Core/Src/stm32f4xx_hal_msp.c"
#elif defined (SYS_TARGET_STM32F405RGT6) || defined (EMULATOR_STM32F405RGT6)
  #include "STM32CubeMX/STM32F405RGT6/Core/Inc/main.h"
  #include "STM32CubeMX/STM32F405RGT6/Core/Src/main.c"
  #include "STM32CubeMX/STM32F405RGT6/Core/Src/stm32f4xx_hal_msp.c"
#endif

#include "device.h"
#include "global.h"
#include "cmn_interrupt.h"


#include "bsp_cpu.h"
#include "bsp_timer.h"
#include "bsp_screen.h"
#include "bsp_rtc.h"
#include "bsp_gyro.h"
#include "app_type.h"
#include "app_rtos.h"
#include "app_lvgl.h"
#include "app_clock.h"
#include "app_cmdbox.h"

#include "FreeRTOS.h"
#include "task.h"


/**
 * @brief
 * @note
 * 
 * 
 * @return
*/
void hw_init(void){
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  bsp_cpu_clock_init();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  MX_DMA_Init();

  MX_USART2_UART_Init();

  MX_SPI1_Init();

  MX_SPI2_Init();

  MX_I2C1_Init();

#if 1
  bsp_timer_init();
#else
  MX_TIM2_Init();
#endif

  MX_TIM3_Init();

  MX_I2C2_Init();

  MX_ADC1_Init();

  MX_USB_OTG_FS_USB_Init();
  
  cmn_interrupt_init_priority();
}




void bsp_init(void){
  bsp_screen_init();
  bsp_qmi8658_init();
  bsp_uart_init();
}


void app_init(void){
#if !(defined EMULATOR_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6)
  app_lvgl_init();
#endif
}



void os_init(void){
  tRtosEvent *p_event = &metope.rtos.event;

  p_event->_handle = xEventGroupCreateStatic( &p_event->_eg_buffer);

#if (defined UNIT_TEST) && (UNIT_TEST==1)
  /**
   * @note
   *  Tasks were defined in the Test Bench 
   */
#else
  tRtosTask  *p_task  = &metope.rtos.task;

  p_task->screen_refresh._handle = xTaskCreateStatic(\
    bsp_screen_main,\
    "bsp_screen_main",\
    sizeof(p_task->screen_refresh._stack) / sizeof(p_task->screen_refresh._stack[0]),\
    &metope.bsp.screen,\
    kAppPriority_VERY_IMPORTANT,\
    &p_task->screen_refresh._stack[0],\
    &p_task->screen_refresh._tcb\
  );
  
  p_task->clock_ui._handle = xTaskCreateStatic(\
    app_clock_main,\
    "app_clock_main",\
    sizeof(p_task->clock_ui._stack) / sizeof(p_task->clock_ui._stack[0]),\
    &metope.app.clock ,\
    kAppPriority_IMPORTANT,\
    &p_task->clock_ui._stack[0],\
    &p_task->clock_ui._tcb\
  );
  
  p_task->screen_onoff._handle = xTaskCreateStatic(\
    bsp_screen_onoff,\
    "bsp_screen_onoff",\
    sizeof(p_task->screen_onoff._stack) / sizeof(p_task->screen_onoff._stack[0]),\
    NULL ,\
    kAppPriority_VERY_IMPORTANT,\
    &p_task->screen_onoff._stack[0],\
    &p_task->screen_onoff._tcb\
  );

  p_task->cmd_box._handle = xTaskCreateStatic(\
    app_cmdbox_main,\
    "app_cmdbox_main",\
    sizeof(p_task->cmd_box._stack) / sizeof(p_task->cmd_box._stack[0]),\
    &metope.bsp.uart ,\
    kAppPriority_NORMAL,\
    &p_task->cmd_box._stack[0],\
    &p_task->cmd_box._tcb\
  );

  TaskHandle_t _handle;
  xTaskCreate( app_rtos_checkpoint, "app_rtos_checkpoint", 256U, NULL, kAppPriority_NORMAL, &_handle);
  
#endif

}








