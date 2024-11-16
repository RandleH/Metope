


#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  #include "stm32f4xx_hal.h"
#endif

#ifdef SYS_TARGET_STM32F411CEU6
  #include "STM32CubeMX/STM32F411CEU6/Core/Inc/main.h"
  #include "STM32CubeMX/STM32F411CEU6/Core/Src/main.c"
  #include "STM32CubeMX/STM32F411CEU6/Core/Src/stm32f4xx_hal_msp.c"
#elif defined SYS_TARGET_STM32F405RGT6
  #include "STM32CubeMX/STM32F405RGT6/Core/Inc/main.h"
  #include "STM32CubeMX/STM32F405RGT6/Core/Src/main.c"
  #include "STM32CubeMX/STM32F405RGT6/Core/Src/stm32f4xx_hal_msp.c"
#endif

#include "global.h"
#include "cmn_device.h"

#if (defined USE_REGISTER) && (USE_REGISTER==1)
  #include "bsp_cpu.h"
#endif

#include "bsp_screen.h"
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

  MX_TIM3_Init();

  MX_USB_OTG_FS_USB_Init();

}




void bsp_init(void){
  bsp_screen_init();
}


#include "app_lvgl.h"
void app_init(void){
  app_lvgl_init();
}



void os_init(void){
  extern TaskHandle_t xTaskCreateStatic(	TaskFunction_t pxTaskCode,
									const char * const pcName,		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
									const uint32_t ulStackDepth,
									void * const pvParameters,
									UBaseType_t uxPriority,
									StackType_t * const puxStackBuffer,
									StaticTask_t * const pxTaskBuffer );


  metope.app.rtos.task.screen_refresh.handle = xTaskCreateStatic(\
    app_task_screen_refresh,\
    "0",\
    sizeof(metope.app.rtos.task.screen_refresh._stack) / sizeof(metope.app.rtos.task.screen_refresh._stack[0]),\
    NULL,\
    kAppPriority_VERY_IMPORTANT,\
    &metope.app.rtos.task.screen_refresh._stack[0],\
    &metope.app.rtos.task.screen_refresh._tcb\
  );
  
}








