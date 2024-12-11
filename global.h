

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


typedef struct stBspScreen{
  bspScreenBrightness_t brightness;
  TickType_t            refresh_rate_ms;
  //...//
} tBspScreen;



typedef struct stAppClock{
  cmnDateTime_t time;
  TaskHandle_t  _handle;
  
  AppGuiClockEnum_t style;
  struct{
    tAppGuiClockParam param;
    void (*init)(tAppGuiClockParam *);
    void (*set_time)(tAppGuiClockParam *, cmnDateTime_t);
    void (*inc_time)(tAppGuiClockParam *, uint32_t);
    void (*deinit)(tAppGuiClockParam *);
  } gui;
} tAppClock;

/**
 * @note
 *  Naming convention: 
 *    - Add '_' if this parameter is ONLY supposed to be overwritten by the third party library.
 *    - Use snake case for the variable names.
 * @todo: Clean up unused terms
 */
typedef struct{
  struct{
    tBspScreen screen;

    struct{
      volatile u32 *_port;
    }led;
  
  }bsp;

  struct{
    SPI_HandleTypeDef  * const pHspi2;
    UART_HandleTypeDef * const pHuart2;
    TIM_HandleTypeDef  * const pHtim3;
    I2C_HandleTypeDef  * const pHi2c1;

    union{
      struct{
        u16 spi2  : 1; /*!< Display Screen */
        u16 uart2 : 1; /*!< Uart Debugging Message */
        u16 tim3  : 1; /*!< System Delay Timer */
        u16 i2c1  : 1; /*!< QMI8658C - I2C Bus */
        u16 B5    : 1; /*!< QMI8658C - INT1 */
        u16 B6    : 1; /*!< QMI8658C - INT2 */
        u16 A9    : 1; /*!< TP_INT - Touch Screen */
        u16 reserved : 10;
      };
      volatile u16 word;
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
          TaskHandle_t _handle;
          StaticTask_t _tcb;
          StackType_t  _stack[APP_CFG_TASK_SCREEN_FRESH_STACK_SIZE];
        }screen_refresh;

        struct{
          TaskHandle_t _handle;
          StaticTask_t _tcb;
          StackType_t  _stack[APP_CFG_TASK_CLOCK_UI_STACK_SIZE];
        }clock_ui;
        
        struct{
          TaskHandle_t _handle;
          StaticTask_t _tcb;
          StackType_t  _stack[APP_CFG_TASK_SCREEN_ONOFF_STACK_SIZE];
        }screen_onoff;
      }task;

      struct{
        StaticEventGroup_t _eg_buffer;  /*!< Event Group Buffer */
        EventGroupHandle_t _handle;     /*!< Event Group Handle */
      }event;

      cmnBoolean_t status; /*!< `ON` | `OFF` */
    }rtos;

    
    tAppClock clock;

  }app;

  struct {
    const cmnDateTime_t system_initial_time;
    union{
      struct{
        u8 display_off : 1;
        u8 reserved    : 7;
      };
      u8 word;
    }status;
  }info;

} tMainSystemStatus;


extern tMainSystemStatus   metope;

extern SPI_HandleTypeDef   hspi2;
extern DMA_HandleTypeDef   hdma_spi2_tx;
extern TIM_HandleTypeDef   htim2;
extern TIM_HandleTypeDef   htim3;
extern UART_HandleTypeDef  huart2;
extern I2C_HandleTypeDef   hi2c1;
extern DMA_HandleTypeDef   hdma_i2c1_rx;
extern DMA_HandleTypeDef   hdma_i2c1_tx;
extern I2C_HandleTypeDef   hi2c2;
extern DMA_HandleTypeDef   hdma_i2c2_rx;
extern DMA_HandleTypeDef   hdma_i2c2_tx;


#ifdef __cplusplus
}
#endif

#if (defined TEST_ONLY) && (TEST_ONLY==1) && (defined __cplusplus)

#include "test.hh"
#include <stdlib.h>
#include "cmn_utility.h"


extern LocalProjectTest     tb_infra_local;
extern HumanInteractionTest tb_infra_bsp;
extern Test                 tb_infra_os;

#endif
#endif