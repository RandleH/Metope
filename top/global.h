/**
 ******************************************************************************
 * @file    global.h
 * @author  RandleH
 * @brief   Global Variable Declaration
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

#ifndef GLOBAL_H
#define GLOBAL_H

#include "device.h"
#include "bsp_type.h"
#include "cmn_type.h"
#include "app_type.h"
#include "lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#ifdef __cplusplus
extern "C"{
#endif


/* ////////////////////////////////////////////////////////////////////////// */
/*                             BSP Screen Objects                             */
/* ////////////////////////////////////////////////////////////////////////// */

typedef union stBspScreenStatusBitmap {
  struct {
    uint16_t is_disp_off : 1;
    uint16_t reserved    : 15;
  };
  uint16_t word;
} tBspScreenStatusBitmap;

typedef struct stBspScreen{
  bspScreenBrightness_t  brightness;
  bspScreenRotate_t      rotation;
  TickType_t             refresh_rate_ms;
  tBspScreenStatusBitmap status;
} tBspScreen;

/* ////////////////////////////////////////////////////////////////////////// */
/*                              BSP Uart Objects                              */
/* ////////////////////////////////////////////////////////////////////////// */
typedef union stBspUartTxStatus {
  struct {
    uint16_t escape_ticks : 8;
    uint16_t msg_len      : 8;
  };
  uint16_t word;
} tBspUartTxStatus;

typedef union stBspUartRxStatus {
  struct {
    uint8_t is_locked     : 1;
    uint8_t is_overflowed : 1;
    uint8_t has_new_msg   : 1;
    uint8_t error_code    : 5;
  };
  uint8_t word;
} tBspUartRxStatus;

typedef struct stBspUart{
  char             tx_buf[BSP_CFG_UART_TX_BUF_SIZE+1];
  tBspUartTxStatus tx_status;
  char             rx_buf[BSP_CFG_UART_RX_BUF_SIZE+1];
  uint8_t          rx_idx;
  tBspUartRxStatus rx_status;
  //...//
} tBspUart;

/* ////////////////////////////////////////////////////////////////////////// */
/*                                 BSP Objects                                */
/* ////////////////////////////////////////////////////////////////////////// */
typedef union stBspStatusBitmap {
  struct{
    uint16_t spi2     : 1; /*!< Display Screen */
    uint16_t uart2    : 1; /*!< Uart Debugging Message */
    uint16_t tim2     : 1; /*!< System Delay Timer 2 */
    uint16_t tim9     : 1; /*!< System Delay Timer 9 */
    uint16_t i2c1     : 1; /*!< QMI8658C - I2C Bus */
    uint16_t B5       : 1; /*!< QMI8658C - INT1 CTRL9 Command Done */
    uint16_t B6       : 1; /*!< QMI8658C - INT2 FIFO Watermark */
    uint16_t A9       : 1; /*!< TP_INT - Touch Screen */
    uint16_t reserved : 8;
  };
  volatile uint16_t word;
} tBspStatusBitmap;

typedef struct stBspStatusBitbandmap {
  uint32_t spi2       [1];
  uint32_t uart2      [1];
  uint32_t tim2       [1];
  uint32_t tim9       [1];
  uint32_t i2c1       [1];
  uint32_t B5         [1];
  uint32_t B6         [1];
  uint32_t A9         [1];
  uint32_t reserved   [8];
} tBspStatusBitbandmap;


typedef struct stBsp {
  SPI_HandleTypeDef  * const pHspi2;
  UART_HandleTypeDef * const pHuart2;
  TIM_HandleTypeDef  * const pHtim3;
  I2C_HandleTypeDef  * const pHi2c1;
  ADC_HandleTypeDef  * const pHadc1;
  
  tBspScreen screen;
  tBspUart   uart;

  tBspStatusBitmap     _status;
  tBspStatusBitbandmap *status;
} tBsp;


/* ========================================================================== */
/*                                 APP Objects                                */
/* ========================================================================== */
typedef struct stAppClockFunc {
  void (*init)(tAppGuiClockParam *);
  void (*set_time)(tAppGuiClockParam *, uint32_t);
  void (*inc_time)(tAppGuiClockParam *, uint32_t);
  void (*idle)(tAppGuiClockParam *);
  void (*deinit)(tAppGuiClockParam *);
} tAppClockFunc;

typedef struct stAppClock{
  TaskHandle_t      _handle;
  AppGuiClockEnum_t style;
  tAppGuiClockParam param;
  tAppClockFunc     func;
} tAppClock;

typedef struct stAppLvgl{
#if LVGL_VERSION==836
  lv_disp_drv_t      disp_drv;
  lv_disp_t         *disp;
  lv_disp_draw_buf_t disp_draw_buf;
  lv_color_t         gram[2][BSP_SCREEN_WIDTH*6];
  cmnBoolean_t       isFlushDone;
#elif LVGL_VERSION==922
  lv_display_t *pDisplayHandle;
  lv_theme_t   *pLvglTheme;
#endif
  lv_obj_t *default_scr;
} tAppLvgl;

/* ========================================================================== */
/*                             APP CmdBox Objects                             */
/* ========================================================================== */
#include "app_cmdbox.h"

/* ************************************************************************** */
/*                             RTOS Task Objects                              */
/* ************************************************************************** */
typedef struct stRtosTask_ScreenRefreash {
  TaskHandle_t _handle;
  StaticTask_t _tcb;
  StackType_t  _stack[APP_CFG_TASK_SCREEN_FRESH_STACK_SIZE];
} tRtosTask_ScreenRefreash;

typedef struct stRtosTask_ClockUi {
  TaskHandle_t _handle;
  StaticTask_t _tcb;
  StackType_t  _stack[APP_CFG_TASK_CLOCK_UI_STACK_SIZE];
} tRtosTask_ClockUi;

typedef struct stRtosTask_ScreenOnOff {
  TaskHandle_t _handle;
  StaticTask_t _tcb;
  StackType_t  _stack[APP_CFG_TASK_SCREEN_ONOFF_STACK_SIZE];
} tRtosTask_ScreenOnOff;

typedef struct stRtosTask_CmdBox {
  TaskHandle_t _handle;
  StaticTask_t _tcb;
  StackType_t  _stack[APP_CFG_TASK_CMD_BOX_STACK_SIZE];
} tRtosTask_CmdBox;

typedef union stRtosTaskIdleBitmap {
  struct{
    uint8_t reserved : 7;
    uint8_t clock    : 1;
  };
  uint8_t word;
} tRtosTaskIdleBitmap;

typedef struct stRtosTask {
  tRtosTask_ScreenRefreash  screen_refresh;
  tRtosTask_ClockUi         clock_ui;
  tRtosTask_ScreenOnOff     screen_onoff;
  tRtosTask_CmdBox          cmd_box;
  tRtosTaskIdleBitmap       bitmap_idle;
} tRtosTask;

/* ************************************************************************** */
/*                             RTOS Event Objects                             */
/* ************************************************************************** */
typedef struct stRtosEvent {
  StaticEventGroup_t _eg_buffer;  /*!< Event Group Buffer */
  EventGroupHandle_t _handle;     /*!< Event Group Handle */
} tRtosEvent;

/* ************************************************************************** */
/*                             RTOS Status Objects                            */
/* ************************************************************************** */
typedef union stRtosStatusBitmap {
  struct {
    uint8_t running  : 1; /*!< `ON` | `OFF` */
    uint8_t stkovfl  : 1; /*!< Stack Overflow */
    uint8_t nomem    : 1; /*!< Out of memory */
    uint8_t reserved : 5;
  };
  uint8_t word;
} tRtosStatusBitmap;

typedef struct stRtosStatusBitbandmap {
  uint32_t running    [1]; /*!< `ON` | `OFF` */
  uint32_t stkovfl    [1]; /*!< Stack Overflow */
  uint32_t nomem      [1]; /*!< Out of memory */
  uint32_t reserved   [5];
} tRtosStatusBitbandmap;

/* ************************************************************************** */
/*                                 RTOS Objects                               */
/* ************************************************************************** */
typedef struct stRtos {
  tRtosTask             task;
  tRtosEvent            event;
  tRtosStatusBitmap     _status;
  tRtosStatusBitbandmap *status;
} tRtos;


typedef struct stApp {
    tAppLvgl   lvgl;
    tAppClock  clock;
    tAppCmdBox cmdbox;
} tApp;

/**
 * @note
 *  Naming convention: 
 *    - Add '_' if this parameter is ONLY supposed to be overwritten by the third party library.
 *    - Use snake case for the variable names.
 * @todo: Clean up unused terms
 */
typedef struct{
  tBsp  bsp;
  tApp  app;
  tRtos rtos;
} tMetope;


extern tMetope metope;

extern ADC_HandleTypeDef   hadc1;
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

#if (defined UNIT_TEST) && (UNIT_TEST==1) && (defined __cplusplus)
  #include "test.hh"
  #include <stdlib.h>
  #include "cmn_utility.h"

  extern LocalProjectTest     tb_infra_local;
  extern HumanInteractionTest tb_infra_bsp;
  extern Test                 tb_infra_os;
#endif


#endif // GLOBAL_H