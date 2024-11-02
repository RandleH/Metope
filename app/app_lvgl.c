/**
 ******************************************************************************
 * @file    app_lvgl.c
 * @author  RandleH
 * @brief   Common Program - LVGL Library Application Interface
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

#include "app_lvgl.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C"{
#endif


const lv_font_t dummy_font;

void app_lvgl_init(void){
  lv_init();
}

#ifdef __cplusplus
}
#endif

