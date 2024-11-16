/**
 ******************************************************************************
 * @file    app_gui.c
 * @author  RandleH
 * @brief   Common Program - GUI Program
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

#include "global.h"
#include "app_gui.h"
#include "cmn_utility.h"

#ifdef __cplusplus
extern "C"{
#endif


void ui_clock1_init(tAppGuiClockParam *pClient)
{
  pClient->pScreen = lv_scr_act();
  lv_obj_clear_flag( pClient->pScreen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_bg_color(pClient->pScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(pClient->pScreen, 0xFF, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_MainPanel = lv_obj_create(pClient->pScreen);
  lv_obj_set_width( ui_MainPanel, 240);
  lv_obj_set_height( ui_MainPanel, 240);
  lv_obj_set_align( ui_MainPanel, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_MainPanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_MainPanel, 120, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_MainPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_MainPanel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_color(ui_MainPanel, lv_color_hex(0xC4C4C4), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_border_opa(ui_MainPanel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(ui_MainPanel, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_side(ui_MainPanel, LV_BORDER_SIDE_FULL, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_PitDigits[6] = {0};
  {
    for( u8 i=0; i<sizeof(ui_PitDigits)/sizeof(*ui_PitDigits); ++i){
      ui_PitDigits[i] = lv_obj_create(pClient->pScreen);
      lv_obj_set_width( ui_PitDigits[i], 2);
      lv_obj_set_height( ui_PitDigits[i], 240);
      lv_obj_set_align( ui_PitDigits[i], LV_ALIGN_CENTER );
      lv_obj_clear_flag( ui_PitDigits[i], LV_OBJ_FLAG_SCROLLABLE );    /// Flags
      lv_obj_set_style_transform_angle(ui_PitDigits[i], i*300, LV_PART_MAIN| LV_STATE_DEFAULT);
      lv_obj_set_style_transform_pivot_x(ui_PitDigits[i], 0, LV_PART_MAIN| LV_STATE_DEFAULT);
      lv_obj_set_style_transform_pivot_y(ui_PitDigits[i], 120, LV_PART_MAIN| LV_STATE_DEFAULT);
    }
  }

  lv_obj_t *ui_InnerLoop = lv_obj_create(pClient->pScreen);
  lv_obj_set_width( ui_InnerLoop, 200);
  lv_obj_set_height( ui_InnerLoop, 200);
  lv_obj_set_align( ui_InnerLoop, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_InnerLoop, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_InnerLoop, 180, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_InnerLoop, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_InnerLoop, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_color(ui_InnerLoop, lv_color_hex(0xADADAD), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_border_opa(ui_InnerLoop, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_color(ui_InnerLoop, lv_color_hex(0xADADAD), LV_PART_MAIN | LV_STATE_DEFAULT );
  // lv_obj_set_style_shadow_opa(ui_InnerLoop, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  // lv_obj_set_style_shadow_width(ui_InnerLoop, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
  // lv_obj_set_style_shadow_spread(ui_InnerLoop, 40, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_PinMin = lv_obj_create(pClient->pScreen);
  lv_obj_set_width( ui_PinMin, 8);
  lv_obj_set_height( ui_PinMin, 88);
  lv_obj_set_x( ui_PinMin, 0 );
  lv_obj_set_y( ui_PinMin, -116 );
  lv_obj_set_align( ui_PinMin, LV_ALIGN_BOTTOM_MID );
  lv_obj_clear_flag( ui_PinMin, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_bg_color(ui_PinMin, lv_color_hex(0x2865CD), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_PinMin, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_color(ui_PinMin, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_border_color(ui_PinMin, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_border_opa(ui_PinMin, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_angle(ui_PinMin, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_pivot_x(ui_PinMin, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_pivot_y(ui_PinMin, 84, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_set_style_bg_color(ui_PinMin, lv_color_hex(0xFFFFFF), LV_PART_SCROLLBAR | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_PinMin, 255, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);
  pClient->pPinMinute = ui_PinMin;


  lv_obj_t *ui_PinHour = lv_obj_create(pClient->pScreen);
  lv_obj_set_width( ui_PinHour, 8);
  lv_obj_set_height( ui_PinHour, 60);
  lv_obj_set_x( ui_PinHour, 0 );
  lv_obj_set_y( ui_PinHour, -116 );
  lv_obj_set_align( ui_PinHour, LV_ALIGN_BOTTOM_MID );
  lv_obj_clear_flag( ui_PinHour, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_bg_color(ui_PinHour, lv_color_hex(0xFF8820), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_PinHour, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_color(ui_PinHour, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_border_opa(ui_PinHour, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_angle(ui_PinHour, 2700, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_pivot_x(ui_PinHour, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_pivot_y(ui_PinHour, 56, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_set_style_border_color(ui_PinHour, lv_color_hex(0x000000), LV_PART_SCROLLBAR | LV_STATE_DEFAULT );
  lv_obj_set_style_border_opa(ui_PinHour, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);

  pClient->pPinHour = ui_PinHour;

  lv_obj_t *ui_Knotch = lv_obj_create(pClient->pScreen);
  lv_obj_set_width( ui_Knotch, 20);
  lv_obj_set_height( ui_Knotch, 20);
  lv_obj_set_align( ui_Knotch, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_Knotch, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_Knotch, 20, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_color(ui_Knotch, lv_color_hex(0x686666), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_main_stop(ui_Knotch, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_stop(ui_Knotch, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_dir(ui_Knotch, LV_GRAD_DIR_VER, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_color(ui_Knotch, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_border_opa(ui_Knotch, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

  // ui_clock1_set_time( pClient, pClient->time);
}


void ui_clock1_set_time(tAppGuiClockParam *pClient, cmnDateTime_t time){
  cmn_utility_angleset( &pClient->_degree_hour, &pClient->_degree_minute, NULL, &time);
  lv_obj_set_style_transform_angle(pClient->pPinHour, pClient->_degree_hour, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_angle(pClient->pPinMinute, pClient->_degree_minute, LV_PART_MAIN| LV_STATE_DEFAULT);
}

/**
 * @todo: Add assertion for microseconds range
 */
void ui_clock1_inc_time(tAppGuiClockParam *pClient, uint32_t ms){
  uint16_t hour_inc, minute_inc;
  cmn_utility_angleinc(&hour_inc, &minute_inc, NULL, ms);

  pClient->_degree_hour += hour_inc;
  pClient->_degree_minute += minute_inc;
  
  lv_obj_set_style_transform_angle(pClient->pPinHour, pClient->_degree_hour, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_angle(pClient->pPinMinute, pClient->_degree_minute, LV_PART_MAIN| LV_STATE_DEFAULT);
}


void ui_clock1_deinit(tAppGuiClockParam *pClient){
  lv_obj_del(pClient->pScreen);
  pClient->pPinHour   = NULL;
  pClient->pPinMinute = NULL;
  pClient->p_anything = NULL;
}

void app_gui_switch( AppGuiClockEnum_t x){
  /**
   * @warning
   *  Assume the `deinit()` is NULL when clock ui is deactivated, vice versa.
   */
  if(NULL!=metope.app.clock.gui.deinit){
    metope.app.clock.gui.deinit( &metope.app.clock.gui.param );
  }
  switch(x){
    case kAppGuiClock_None:{
      metope.app.clock.gui.init     = NULL;
      metope.app.clock.gui.set_time = NULL;
      metope.app.clock.gui.inc_time = NULL;
      metope.app.clock.gui.deinit   = NULL;
      break;
    }
    case kAppGuiClock_Clock1:{
      metope.app.clock.gui.init     = ui_clock1_init;
      metope.app.clock.gui.set_time = ui_clock1_set_time;
      metope.app.clock.gui.inc_time = ui_clock1_inc_time;
      metope.app.clock.gui.deinit   = ui_clock1_deinit;
      break;
    }
    default:{
      /**
       * @todo: Add assertion
       */
      break;
    }
  }

  metope.app.clock.gui.init( &metope.app.clock.gui.param );
}

#ifdef __cplusplus
}
#endif
