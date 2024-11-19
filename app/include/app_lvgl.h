/**
 ******************************************************************************
 * @file    app_lvgl.h
 * @author  RandleH
 * @brief   Application Program - LVGL Library Application Interface
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


#include <stdarg.h>
#include "cmn_type.h"

#ifndef APP_LVGL_H
#define APP_LVGL_H

#ifdef __cplusplus
extern "C"{
#endif

u32 app_lvgl_get_tick(void);

void app_lvgl_init(void);
void app_lvgl_flush_all( void);

int app_lvgl_snprintf(char * buffer, size_t count, const char * format, ...);
int app_lvgl_vsnprintf(char * buffer, size_t count, const char * format, va_list va);

#ifdef __cplusplus
}
#endif

#endif