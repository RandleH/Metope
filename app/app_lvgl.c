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

  bsp_screen_refresh( (bspScreenPixel_t *)buf, area->x1, area->y1, area->x2, area->y2);

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
 * @brief
 * @addtogroup MachineDependent
 */
void app_lvgl_init(void){
  lv_init();
  
#if LVGL_VERSION==836
  lv_disp_drv_init( &THIS->lvgl.disp_drv);
  THIS->lvgl.disp_drv.draw_buf    = &THIS->lvgl.disp_draw_buf;
  THIS->lvgl.disp_drv.flush_cb    = app_lvgl_flush_cb;
  THIS->lvgl.disp_drv.hor_res     = BSP_SCREEN_HEIGHT;
  THIS->lvgl.disp_drv.ver_res     = BSP_SCREEN_WIDTH;
  THIS->lvgl.disp_drv.direct_mode = false;
  lv_disp_draw_buf_init( &THIS->lvgl.disp_draw_buf, THIS->lvgl.gram[0], THIS->lvgl.gram[1], sizeof(THIS->lvgl.gram[0])/sizeof(THIS->lvgl.gram[0][0]));

  THIS->lvgl.disp = lv_disp_drv_register( &THIS->lvgl.disp_drv);

  lv_disp_t *dispp = lv_disp_get_default();
  lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
  lv_disp_set_theme(dispp, theme);

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

  THIS->lvgl.default_scr = lv_scr_act();
}


void app_lvgl_flush_all( void){
  do{
    lv_timer_handler();
    HAL_Delay(10);
  }while(!THIS->lvgl.isFlushDone);
}


void app_lvgl_load_default_screen(void){
  // lv_scr_load(THIS->lvgl.default_scr);
}


/* ************************************************************************** */
/*                                 Generated                                  */
/* ************************************************************************** */
// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.0
// LVGL version: 8.3.6
// Project name: ModernClock

void _ui_bar_set_property( lv_obj_t *target, int id, int val) 
{
   if (id == _UI_BAR_PROPERTY_VALUE_WITH_ANIM) lv_bar_set_value(target, val, LV_ANIM_ON);
   if (id == _UI_BAR_PROPERTY_VALUE) lv_bar_set_value(target, val, LV_ANIM_OFF);
}

void _ui_basic_set_property( lv_obj_t *target, int id, int val) 
{
   if (id == _UI_BASIC_PROPERTY_POSITION_X) lv_obj_set_x(target, val);
   if (id == _UI_BASIC_PROPERTY_POSITION_Y) lv_obj_set_y(target, val);
   if (id == _UI_BASIC_PROPERTY_WIDTH) lv_obj_set_width(target, val);
   if (id == _UI_BASIC_PROPERTY_HEIGHT) lv_obj_set_height(target, val);
}

void _ui_dropdown_set_property( lv_obj_t *target, int id, int val) 
{
   if (id == _UI_DROPDOWN_PROPERTY_SELECTED) lv_dropdown_set_selected(target, val);
}

void _ui_image_set_property( lv_obj_t *target, int id, uint8_t *val) 
{
   if (id == _UI_IMAGE_PROPERTY_IMAGE) lv_img_set_src(target, val);
}

void _ui_label_set_property( lv_obj_t *target, int id, const char *val) 
{
   if (id == _UI_LABEL_PROPERTY_TEXT) lv_label_set_text(target, val);
}

void _ui_roller_set_property( lv_obj_t *target, int id, int val) 
{
   if (id == _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM) lv_roller_set_selected(target, val, LV_ANIM_ON);
   if (id == _UI_ROLLER_PROPERTY_SELECTED) lv_roller_set_selected(target, val, LV_ANIM_OFF);
}

void _ui_slider_set_property( lv_obj_t *target, int id, int val) 
{
   if (id == _UI_SLIDER_PROPERTY_VALUE_WITH_ANIM) lv_slider_set_value(target, val, LV_ANIM_ON);
   if (id == _UI_SLIDER_PROPERTY_VALUE) lv_slider_set_value(target, val, LV_ANIM_OFF);
}

void _ui_screen_change( lv_obj_t ** target, lv_scr_load_anim_t fademode, int spd, int delay, void (*target_init)(void)) 
{
   if(*target == NULL)
      target_init();
   lv_scr_load_anim(*target, fademode, spd, delay, false);
}

void _ui_screen_delete( lv_obj_t ** target ) 
{
   if(*target == NULL)
   {
      lv_obj_del(*target);
      target = NULL;
   }
}

void _ui_arc_increment( lv_obj_t *target, int val) 
{
   int old = lv_arc_get_value(target);
   lv_arc_set_value(target, old+val);
   lv_event_send(target,LV_EVENT_VALUE_CHANGED, 0);
}

void _ui_bar_increment( lv_obj_t *target, int val, int anm) 
{
   int old = lv_bar_get_value(target);
   lv_bar_set_value(target, old+val, anm);
}

void _ui_slider_increment( lv_obj_t *target, int val, int anm) 
{
   int old = lv_slider_get_value(target);
   lv_slider_set_value(target, old+val, anm);
   lv_event_send(target,LV_EVENT_VALUE_CHANGED, 0);
}

void _ui_keyboard_set_target( lv_obj_t *keyboard, lv_obj_t *textarea) 
{
   lv_keyboard_set_textarea(keyboard, textarea);
}

void _ui_flag_modify( lv_obj_t *target, int32_t flag, int value)
{
    if (value==_UI_MODIFY_FLAG_TOGGLE)
    {
        if ( lv_obj_has_flag(target,flag) ) lv_obj_clear_flag(target,flag);
        else lv_obj_add_flag(target,flag);
    }
    else if (value==_UI_MODIFY_FLAG_ADD) lv_obj_add_flag(target,flag);
    else lv_obj_clear_flag(target,flag);
}
void _ui_state_modify( lv_obj_t *target, int32_t state, int value)
{
    if (value==_UI_MODIFY_STATE_TOGGLE)
    {
        if ( lv_obj_has_state(target,state) ) lv_obj_clear_state(target,state);
        else lv_obj_add_state(target,state);
    }
    else if (value==_UI_MODIFY_STATE_ADD) lv_obj_add_state(target,state);
    else lv_obj_clear_state(target,state);
}

void _ui_textarea_move_cursor(lv_obj_t * target, int val)
{
    if (val==UI_MOVE_CURSOR_UP) lv_textarea_cursor_up(target);
    if (val==UI_MOVE_CURSOR_RIGHT) lv_textarea_cursor_right(target);
    if (val==UI_MOVE_CURSOR_DOWN) lv_textarea_cursor_down(target);
    if (val==UI_MOVE_CURSOR_LEFT) lv_textarea_cursor_left(target);
    lv_obj_add_state(target, LV_STATE_FOCUSED);
}

void scr_unloaded_delete_cb(lv_event_t * e)
{
   lv_obj_t ** var = lv_event_get_user_data(e);
   lv_obj_del(*var);
   (*var) = NULL;
}

void _ui_opacity_set( lv_obj_t *target, int val) 
{
   lv_obj_set_style_opa(target, val, 0);
}

void _ui_anim_callback_free_user_data(lv_anim_t *a)
{
	lv_mem_free(a->user_data);
	a->user_data=NULL;
}

void _ui_anim_callback_set_x(lv_anim_t* a, int32_t v)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   lv_obj_set_x(usr->target, v);
}

void _ui_anim_callback_set_y(lv_anim_t* a, int32_t v)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   lv_obj_set_y(usr->target, v);
}

void _ui_anim_callback_set_width(lv_anim_t* a, int32_t v)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   lv_obj_set_width(usr->target, v);
}

void _ui_anim_callback_set_height(lv_anim_t* a, int32_t v)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   lv_obj_set_height(usr->target, v);
}

void _ui_anim_callback_set_opacity(lv_anim_t* a, int32_t v)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   lv_obj_set_style_opa(usr->target, v, 0);
}

void _ui_anim_callback_set_image_zoom(lv_anim_t* a, int32_t v)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   lv_img_set_zoom(usr->target, v);
}

void _ui_anim_callback_set_image_angle(lv_anim_t* a, int32_t v)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   lv_img_set_angle(usr->target, v);
}

void _ui_anim_callback_set_image_frame(lv_anim_t* a, int32_t v)
{
    ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
    usr->val = v;
    if ( v<0 ) v=0;
    if ( v>=usr->imgset_size ) v=usr->imgset_size-1;
    lv_img_set_src(usr->target, usr->imgset[v]);
}

int32_t _ui_anim_callback_get_x(lv_anim_t* a)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   return lv_obj_get_x_aligned(usr->target);
}

int32_t _ui_anim_callback_get_y(lv_anim_t* a)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   return lv_obj_get_y_aligned(usr->target);
}

int32_t _ui_anim_callback_get_width(lv_anim_t* a)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   return lv_obj_get_width(usr->target);
}

int32_t _ui_anim_callback_get_height(lv_anim_t* a)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   return lv_obj_get_height(usr->target);
}

int32_t _ui_anim_callback_get_opacity(lv_anim_t* a)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   return lv_obj_get_style_opa(usr->target, 0);
}

int32_t _ui_anim_callback_get_image_zoom(lv_anim_t* a)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   return lv_img_get_zoom(usr->target);
}

int32_t _ui_anim_callback_get_image_angle(lv_anim_t* a)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   return lv_img_get_angle(usr->target);
}

int32_t _ui_anim_callback_get_image_frame(lv_anim_t* a)
{
   ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
   return usr->val;
}

void _ui_arc_set_text_value( lv_obj_t *trg, lv_obj_t *src, const char *prefix, const char *postfix) 
{
   char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE];
   lv_snprintf(buf, sizeof(buf), "%s%d%s", prefix, (int)lv_arc_get_value(src), postfix);
   lv_label_set_text(trg, buf);
}

void _ui_slider_set_text_value( lv_obj_t *trg, lv_obj_t *src, const char *prefix, const char *postfix) 
{
   char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE];
   lv_snprintf(buf, sizeof(buf), "%s%d%s", prefix, (int)lv_slider_get_value(src), postfix);
   lv_label_set_text(trg, buf);
}
void _ui_checked_set_text_value( lv_obj_t *trg, lv_obj_t *src, const char *txt_on, const char *txt_off) 
{
    if (lv_obj_has_state(src,LV_STATE_CHECKED)) lv_label_set_text(trg,txt_on);
    else lv_label_set_text(trg,txt_off);
}

void _ui_spinbox_step(lv_obj_t * target, int val)
{
	if(val > 0) lv_spinbox_increment(target);
	else lv_spinbox_decrement(target);

	lv_event_send(target,LV_EVENT_VALUE_CHANGED, 0);
}

void _ui_switch_theme(int val)
{
#ifdef UI_THEME_ACTIVE
    ui_theme_set(val);
#endif
}

#ifdef __cplusplus
}
#endif

/* ********************************** EOF *********************************** */
