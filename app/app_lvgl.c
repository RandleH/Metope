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

/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include <stdlib.h>
#include "app_lvgl.h"
#include "lvgl.h"
#include "global.h"
#include "device.h"
#include "bsp_type.h"
#include "bsp_screen.h"

/* ************************************************************************** */
/*                               Private Macros                               */
/* ************************************************************************** */
#define THIS (&metope.app)


/* ************************************************************************** */
/*                             Private Functions                              */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

#if LVGL_VERSION==836
STATIC void app_lvgl_flush_cb(struct _lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *buf);
#elif LVGL_VERSION==922
STATIC void app_lvgl_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
#endif

#if LVGL_VERSION==836
STATIC void app_lvgl_flush_cb(struct _lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *buf){
#elif LVGL_VERSION==922
STATIC void app_lvgl_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
#endif
  THIS->lvgl.isFlushDone = lv_disp_flush_is_last(disp);

  // bsp_screen_refresh( (bspScreenPixel_t *)buf, area->x1, area->y1, area->x2, area->y2);

#if LVGL_VERSION==836
  lv_disp_flush_ready(disp);
#elif LVGL_VERSION==922
  lv_display_flush_ready(disp);
#endif
}

#ifdef __cplusplus
}
#endif


/* ************************************************************************** */
/*                              Public Functions                              */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

const lv_font_t dummy_font;

/**
 * @brief
 * @note DummyFunction
 */
int app_lvgl_snprintf(char * buffer, size_t count, const char * format, ...){
  if(buffer){
    buffer[0] = '\0';
  }
  return 0;
}

/**
 * @brief
 * @note DummyFunction
 */
int app_lvgl_vsnprintf(char * buffer, size_t count, const char * format, va_list va){
  if(buffer){
    buffer[0] = '\0';
  }
  return 0;
}


uint32_t app_lvgl_get_tick(void){
  return HAL_GetTick();
}

/**
 * @brief Malloc Wrapper Function
 * 
 */
void *app_lvgl_malloc(size_t xWantedSize) {
  extern void *pvPortMalloc( size_t xWantedSize );
  const tRtos *p_rtos = &THIS->rtos;
  if (p_rtos->status.running) {
    return pvPortMalloc(xWantedSize);
  }else {
    return malloc(xWantedSize);
  }
}

void app_lvgl_free(void *pv) {
  extern void vPortFree( void *pv );
  const tRtos *p_rtos = &THIS->rtos;
  if (p_rtos->status.running) {
    vPortFree(pv);
  }else {
    free(pv);
  }
}

void *app_lvgl_realloc(void *pv, size_t xWantedSize) {
  extern void *pvPortRealloc( void *pv, size_t xWantedSize );
  const tRtos *p_rtos = &THIS->rtos;
  if (p_rtos->status.running) {
    return pvPortRealloc(pv, xWantedSize);
  }else {
    return realloc(pv, xWantedSize);
  }
}

/**
 * @brief
 * @addtogroup MachineDependent
 */
void app_lvgl_init(void){
  lv_init();
  
#if LVGL_VERSION==836
  // lv_disp_drv_init( &THIS->lvgl.disp_drv);
  // THIS->lvgl.disp_drv.draw_buf    = &THIS->lvgl.disp_draw_buf;
  // THIS->lvgl.disp_drv.flush_cb    = app_lvgl_flush_cb;
  // THIS->lvgl.disp_drv.hor_res     = BSP_SCREEN_HEIGHT;
  // THIS->lvgl.disp_drv.ver_res     = BSP_SCREEN_WIDTH;
  // THIS->lvgl.disp_drv.direct_mode = false;
  // lv_disp_draw_buf_init( &THIS->lvgl.disp_draw_buf, THIS->lvgl.gram[0], THIS->lvgl.gram[1], sizeof(THIS->lvgl.gram[0])/sizeof(THIS->lvgl.gram[0][0]));

  // THIS->lvgl.disp = lv_disp_drv_register( &THIS->lvgl.disp_drv);

  // lv_disp_t *dispp = lv_disp_get_default();
  // lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
  // lv_disp_set_theme(dispp, theme);

#elif LVGL_VERSION==922

  /* Tick Interface */
  lv_tick_set_cb(HAL_GetTick);

  /* Display Setup */
  THIS->lvgl.pDisplayHandle = lv_display_create(BSP_SCREEN_WIDTH, BSP_SCREEN_HEIGHT);
  lv_display_set_default(THIS->lvgl.pDisplayHandle);
  
  /* Draw Buffer */
  lv_display_set_buffers(THIS->lvgl.pDisplayHandle, gram[0], gram[1], sizeof(gram[0]), LV_DISPLAY_RENDER_MODE_PARTIAL);
  
  /* Set Resolution */
  lv_display_set_resolution( THIS->lvgl.pDisplayHandle, BSP_SCREEN_WIDTH, BSP_SCREEN_HEIGHT);
  lv_display_set_physical_resolution( THIS->lvgl.pDisplayHandle, -1, -1);

  /* Color Format */
  lv_display_set_color_format( THIS->lvgl.pDisplayHandle, LV_COLOR_FORMAT_RGB565);
  
  /* Flush Call Back */
  lv_display_set_flush_cb( THIS->lvgl.pDisplayHandle, app_lvgl_flush_cb);

  THIS->lvgl.pLvglTheme = lv_theme_default_init( THIS->lvgl.pDisplayHandle, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
  lv_disp_set_theme( THIS->lvgl.pDisplayHandle, THIS->lvgl.pLvglTheme );

#endif

  // THIS->lvgl.default_scr = lv_scr_act();
}


void app_lvgl_flush_all( void){
  do{
    lv_timer_handler();
    HAL_Delay(10);
  }while(!THIS->lvgl.isFlushDone);
}


void app_lvgl_load_default_screen(void){
  lv_scr_load(THIS->lvgl.default_scr);
}


#ifdef __cplusplus
}
#endif

/* ********************************** EOF *********************************** */
