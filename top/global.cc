/**
 ******************************************************************************
 * @file    global.cc
 * @author  RandleH
 * @brief   Global Variable Initialization (Default Setting)
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
#include "global.h"



#ifdef __cplusplus
extern "C"{
#endif

tMetope  metope = {
  .bsp = {
    .pHspi2  = &hspi2,
    .pHuart2 = &huart2,
    .pHtim3  = &htim3,
    .pHi2c1  = &hi2c1,
    .pHadc1  = &hadc1,
    .screen = {
      .brightness      = BSP_SCREEN_DEFAULT_BRIGHTNESS,
      .refresh_rate_ms = BSP_SCREEN_DEFAULT_REFREASHRATE
    },
    .status  = {.word=0}
  },
  .app = {
    .lvgl = {
#if LVGL_VERSION==836
      .disp_drv      = {0},
      .disp          = NULL,
      .disp_draw_buf = {0},
      .gram          = {0},
      .isFlushDone   = true,
#elif LVGL_VERSION==922
      .pDisplayHandle = NULL,
      .pLvglTheme     = NULL
#endif
      .default_scr  = NULL
    },
    .clock = {
      ._handle = NULL,
      .style   = kAppGuiClock_ClockModern
    }
  },
  .rtos = {.status = {0}},
  .info = {
    .status = {
      {
        .scroff   = 0,
        .reserved = 0
      }
    }
  }
};

volatile u32 gDummy = 0;

#ifdef __cplusplus
}
#endif


#if (defined UNIT_TEST) && (UNIT_TEST==1) && (defined __cplusplus)
  #if (defined INCLUDE_TB_CMN) && (INCLUDE_TB_CMN==1)
    LocalProjectTest     tb_infra_local;
  #endif

  #if (defined INCLUDE_TB_BSP) && (INCLUDE_TB_BSP==1)
    HumanInteractionTest tb_infra_bsp;
  #endif

  #if (defined INCLUDE_TB_OS) && (INCLUDE_TB_OS==1)
    Test                 tb_infra_os;
  #endif

#endif
