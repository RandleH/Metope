




/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#ifdef SYS_TARGET_STM32F411CEU6
  #include "STM32CubeMX/STM32F411CEU6/Core/Inc/main.h"
  #include "STM32CubeMX/STM32F411CEU6/Core/Src/main.c"
  #include "STM32CubeMX/STM32F411CEU6/Core/Src/stm32f4xx_hal_msp.c"
#elif defined SYS_TARGET_STM32F405RGT6
  #include "STM32CubeMX/STM32F405RGT6/Core/Inc/main.h"
  #include "STM32CubeMX/STM32F405RGT6/Core/Src/main.c"
  #include "STM32CubeMX/STM32F405RGT6/Core/Src/stm32f4xx_hal_msp.c"
#endif

#include "device.h"
#include "global.h"
#include "cmn_interrupt.h"

#if (defined USE_REGISTER) && (USE_REGISTER==1)
  #include "bsp_cpu.h"
  #include "bsp_timer.h"
#endif

#include "bsp_screen.h"
#include "bsp_rtc.h"
#include "bsp_gyro.h"
#include "app_type.h"
#include "app_task.h"

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
  
  /* Init the low level hardware */
  HAL_MspInit();

#if (defined USE_REGISTER) && (USE_REGISTER==1)
  bsp_cpu_clock_init();
#else
  /* Configure the system clock */
  SystemClock_Config();
#endif

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  MX_DMA_Init();

  MX_USART2_UART_Init();

  MX_SPI1_Init();

  MX_SPI2_Init();

  MX_I2C1_Init();

#if (defined USE_REGISTER) && (USE_REGISTER==1)
  bsp_timer_init();
#else
  MX_TIM2_Init();
#endif

  MX_TIM3_Init();

  MX_I2C2_Init();

  MX_USB_OTG_FS_USB_Init();
  
  cmn_interrupt_init_priority();
}




void bsp_init(void){
  bsp_screen_init();
#if (defined TEST_ONLY) && (TEST_ONLY==1)
  /**
   * @note: Module need to initialize ONLY in test mode
   */
  bsp_rtc_init();
#else
  /**
   * @note: Module will be initialized during the test
   */
  bsp_qmi8658_init();
#endif

}

#include "app_lvgl.h"
void app_init(void){

#if ((!(defined TEST_ONLY)) || (TEST_ONLY==0)) || ((defined TEST_ONLY) && (TEST_ONLY==1) && (defined INCLUDE_TB_OS) && (INCLUDE_TB_OS==1))
  app_lvgl_init();
#endif
}


#include "app_gui.h"
void os_init(void){
  metope.app.rtos.event._handle = xEventGroupCreateStatic( &metope.app.rtos.event._eg_buffer);

#if (defined TEST_ONLY) && (TEST_ONLY==1)
  /**
   * @note
   *  Tasks were defined in the Test Bench 
   */
#else

  metope.app.rtos.task.screen_refresh._handle = xTaskCreateStatic(\
    bsp_screen_main,\
    "0",\
    sizeof(metope.app.rtos.task.screen_refresh._stack) / sizeof(metope.app.rtos.task.screen_refresh._stack[0]),\
    &metope.bsp.screen,\
    kAppPriority_VERY_IMPORTANT,\
    &metope.app.rtos.task.screen_refresh._stack[0],\
    &metope.app.rtos.task.screen_refresh._tcb\
  );
  
  metope.app.rtos.task.clock_ui._handle = xTaskCreateStatic(\
    app_clock_gui_main,\
    "0",\
    sizeof(metope.app.rtos.task.clock_ui._stack) / sizeof(metope.app.rtos.task.clock_ui._stack[0]),\
    &metope.app.clock ,\
    kAppPriority_VERY_IMPORTANT,\
    &metope.app.rtos.task.clock_ui._stack[0],\
    &metope.app.rtos.task.clock_ui._tcb\
  );
  
  metope.app.rtos.task.screen_onoff._handle = xTaskCreateStatic(\
    bsp_screen_onoff,\
    "0",\
    sizeof(metope.app.rtos.task.screen_onoff._stack) / sizeof(metope.app.rtos.task.screen_onoff._stack[0]),\
    NULL ,\
    kAppPriority_VERY_IMPORTANT,\
    &metope.app.rtos.task.screen_onoff._stack[0],\
    &metope.app.rtos.task.screen_onoff._tcb\
  );
#endif

}








