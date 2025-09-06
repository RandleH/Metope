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
#include <stdint.h>
#include <stddef.h>

#ifndef APP_LVGL_H
#define APP_LVGL_H

#ifdef __cplusplus
extern "C"{
#endif

uint32_t app_lvgl_get_tick(void);

void app_lvgl_init(void);
void app_lvgl_flush_all( void);
void app_lvgl_load_default_screen(void);

int app_lvgl_snprintf(char * buffer, size_t count, const char * format, ...);
int app_lvgl_vsnprintf(char * buffer, size_t count, const char * format, va_list va);

void *app_lvgl_malloc(size_t xWantedSize);
void app_lvgl_free(void *pv);
void *app_lvgl_realloc(void *pv, size_t xWantedSize);
#ifdef __cplusplus
}
#endif

#endif