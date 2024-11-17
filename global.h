

#ifndef GLOBAL_H
#define GLOBAL_H

#include "bsp_type.h"
#include "cmn_type.h"
#include "app_type.h"
#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  #include "stm32f4xx_hal.h"
#endif
#include "lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @note
 *  Naming convention: 
 *    - Add '_' if this parameter is ONLY supposed to be overwritten by the third party library.
 *    - Use snake case for the variable names.
 * @todo: Clean up unused terms
 */
typedef struct{
  struct{
    struct{
      bspScreenBrightness_t _brightness;
      //...//
    }screen;

    struct{
      volatile u32 *_port;
    }led;
  
  }bsp;

  struct{
    SPI_HandleTypeDef  * const pHspi2;
    UART_HandleTypeDef * const pHuart2;
    TIM_HandleTypeDef  * const pHtim3;

    union{
      struct{
        u16 spi2  : 1;
        u16 uart2 : 1;
        u16 tim3  : 1;
        u16 reserved : 13;
      };
      u16 word;
    }status;
    //...//
  }dev;
  
  struct{
    struct{
#if LVGL_VERSION==836
      lv_disp_drv_t      disp_drv;
      lv_disp_t         *disp;
      lv_disp_draw_buf_t disp_draw_buf;
      lv_color_t         gram[2][BSP_SCREEN_WIDTH*4];
      cmnBoolean_t       isFlushDone;
#elif LVGL_VERSION==922
      lv_display_t *pDisplayHandle;
      lv_theme_t   *pLvglTheme;
#endif
    }lvgl;

    struct{
      struct{
        struct{
          TaskHandle_t handle;
          StaticTask_t _tcb;
          StackType_t  _stack[APP_CFG_TASK_SCREEN_FRESH_STACK_SIZE];    
        }screen_refresh;

      }task;

      struct{
        StaticEventGroup_t _eg_buffer;  /*!< Event Group Buffer */
        EventGroupHandle_t _handle;     /*!< Event Group Handle */
      }event;

      cmnBoolean_t status; /*!< `ON` | `OFF` */
    }rtos;

    struct{
      struct{
        tAppGuiClockParam param;
        void (*init)(tAppGuiClockParam *);
        void (*set_time)(tAppGuiClockParam *, cmnDateTime_t);
        void (*inc_time)(tAppGuiClockParam *, uint32_t);
        void (*deinit)(tAppGuiClockParam *);
      }gui;
      cmnDateTime_t time;
      TaskHandle_t _handle;
    }clock;
  }app;

  struct {
    const cmnDateTime_t system_initial_time;
  }info;

} tMainSystemStatus;


extern tMainSystemStatus   metope;

extern SPI_HandleTypeDef   hspi2;
extern DMA_HandleTypeDef   hdma_spi2_tx;
extern TIM_HandleTypeDef   htim2;
extern TIM_HandleTypeDef   htim3;
extern UART_HandleTypeDef  huart2;


#ifdef __cplusplus
}
#endif

#if (defined TEST_ONLY) && (TEST_ONLY==1) && (defined __cplusplus)

#include "test.hh"
#include <stdlib.h>
#include "cmn_utility.h"


extern LocalProjectTest     tb_infra_local;
extern HumanInteractionTest tb_infra_hmi;
extern Test                 tb_infra_os;

#endif
#endif