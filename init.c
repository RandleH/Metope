


#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
#include "stm32f4xx_hal.h"
#endif

#include "cmn_device.h"

#ifdef SYS_TARGET_STM32F411CEU6
#include "STM32CubeMX/STM32F411CEU6/Core/Inc/main.h"
#include "STM32CubeMX/STM32F411CEU6/Core/Src/main.c"
#include "STM32CubeMX/STM32F411CEU6/Core/Src/stm32f4xx_hal_msp.c"
#elif defined SYS_TARGET_STM32F405RGT6
#include "STM32CubeMX/STM32F405RGT6/Core/Inc/main.h"
#include "STM32CubeMX/STM32F405RGT6/Core/Src/main.c"
#include "STM32CubeMX/STM32F405RGT6/Core/Src/stm32f4xx_hal_msp.c"
#endif



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

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  MX_USB_OTG_FS_USB_Init();

}











