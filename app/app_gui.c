/**
 ******************************************************************************
 * @file    app_gui.c
 * @author  RandleH
 * @brief   Application Program - GUI Program
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
#include "assert.h"
#include "trace.h"
#include "app_gui.h"
#include "cmn_utility.h"
#include "app_gui_asset"
#include "bsp_rtc.h"

/* ************************************************************************** */
/*                               Private Macros                               */
/* ************************************************************************** */
#ifdef DEBUG
  #define VOLATILE volatile
#else
  #define VOLATILE
#endif

/* ************************************************************************** */
/*                     Static Clock UI Function - clock1                      */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief UI Clock1 Initialization
 * @param [out] pClient - The UI Widget Structure Variable
 * @note UI Clock1 has no special parameters. `pClient->p_anything` will set to `NULL`
 */
static void ui_clock1_init(tAppGuiClockParam *pClient)
{
  pClient->pScreen = lv_scr_act();
  lv_obj_clear_flag( pClient->pScreen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_bg_color(pClient->pScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(pClient->pScreen, 0xFF, LV_PART_MAIN| LV_STATE_DEFAULT);

  /**
   * @note: Main Panel
   */
  {
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
  }

  /**
   * @note: Clock Panel Digits
   */
  {
    lv_obj_t *ui_PitDigits[6] = {0};
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

  /**
   * @note: Inner Loop Ornament
   */
  {  
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
  }

  /**
   * @note: Vivinne Westwood
   */
  {
    lv_obj_t* ui_vvw = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_vvw, 26);
    lv_obj_set_height( ui_vvw, 28);
    lv_obj_set_x( ui_vvw, 0 );
    lv_obj_set_y( ui_vvw, -35 );
    lv_obj_set_align( ui_vvw, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_vvw, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_vvw, 90, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_vvw, lv_color_hex(0xE4E4E4), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_vvw, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_vvw, lv_color_hex(0x484848), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_grad_dir(ui_vvw, LV_GRAD_DIR_VER, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_vvw, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_vvw, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_vvw, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  
    lv_obj_t* ui_vvwc = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_vvwc, 36);
    lv_obj_set_height( ui_vvwc, 8);
    lv_obj_set_x( ui_vvwc, 0 );
    lv_obj_set_y( ui_vvwc, -35 );
    lv_obj_set_align( ui_vvwc, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_vvwc, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_vvwc, 90, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_vvwc, lv_color_hex(0x4C4C4C), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_vvwc, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_vvwc, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_vvwc, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_vvwc, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    
    lv_obj_t* ui_vvwc2 = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_vvwc2, 49);
    lv_obj_set_height( ui_vvwc2, 4);
    lv_obj_set_x( ui_vvwc2, 0 );
    lv_obj_set_y( ui_vvwc2, -35 );
    lv_obj_set_align( ui_vvwc2, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_vvwc2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_vvwc2, 90, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_vvwc2, lv_color_hex(0x5B5B5B), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_vvwc2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_vvwc2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_vvwc2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_vvwc2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    
    lv_obj_t* ui_vvwc1 = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_vvwc1, 65);
    lv_obj_set_height( ui_vvwc1, 2);
    lv_obj_set_x( ui_vvwc1, 0 );
    lv_obj_set_y( ui_vvwc1, -35 );
    lv_obj_set_align( ui_vvwc1, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_vvwc1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_vvwc1, 90, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_vvwc1, lv_color_hex(0x6C6C69), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_vvwc1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_vvwc1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_vvwc1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_vvwc1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    
    lv_obj_t* ui_vvwh = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_vvwh, 18);
    lv_obj_set_height( ui_vvwh, 8);
    lv_obj_set_x( ui_vvwh, 0 );
    lv_obj_set_y( ui_vvwh, -57 );
    lv_obj_set_align( ui_vvwh, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_vvwh, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_vvwh, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_vvwh, lv_color_hex(0xBDBDBD), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_vvwh, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_vvwh, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_vvwh, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_vvwh, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    
    lv_obj_t* ui_vvwv = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_vvwv, 8);
    lv_obj_set_height( ui_vvwv, 18);
    lv_obj_set_x( ui_vvwv, 0 );
    lv_obj_set_y( ui_vvwv, -57 );
    lv_obj_set_align( ui_vvwv, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_vvwv, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_vvwv, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_vvwv, lv_color_hex(0xBDBDBD), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_vvwv, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_vvwv, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_vvwv, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_vvwv, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t* ui_vvwvo = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_vvwvo, 8);
    lv_obj_set_height( ui_vvwvo, 8);
    lv_obj_set_x( ui_vvwvo, 0 );
    lv_obj_set_y( ui_vvwvo, -57 );
    lv_obj_set_align( ui_vvwvo, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_vvwvo, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_vvwvo, 8, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_vvwvo, lv_color_hex(0x780B0B), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_vvwvo, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_vvwvo, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_vvwvo, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_vvwvo, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  }

  /**
   * @note: Minute Hand Needle
   */
  {
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
  }
  
  /**
   * @note: Hour Hand Needle
   */
  {
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
  }


  /**
   * @note: Knotch
   */
  {
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
  }

}

/**
 * @brief UI Clock1 Set Time
 * @param [inout] pClient - The UI Widget Structure Variable
 * @note Update needle angle
 */
static void ui_clock1_set_time(tAppGuiClockParam *pClient, cmnDateTime_t time){
  cmn_utility_angleset( &pClient->_rem_hour, &pClient->_rem_minute, NULL, &pClient->_degree_hour, &pClient->_degree_minute, NULL, &time);
  
  lv_obj_set_style_transform_angle(pClient->pPinHour, pClient->_degree_hour, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_angle(pClient->pPinMinute, pClient->_degree_minute, LV_PART_MAIN| LV_STATE_DEFAULT);
}

/**
 * @brief UI Clock1 Inc Time (ms)
 * @param [inout] pClient - The UI Widget Structure Variable
 * @note Update needle angle
 */
static void ui_clock1_inc_time(tAppGuiClockParam *pClient, uint32_t ms){
  uint16_t hour_inc, minute_inc;
  cmn_utility_angleinc( &pClient->_rem_hour, &pClient->_rem_minute, NULL, &hour_inc, &minute_inc, NULL, ms);

  pClient->_degree_hour += hour_inc;
  pClient->_degree_minute += minute_inc;

  /**
   * @note
   *  The angle which the minute hand has swept is always 12 times of the hour hand interval.
   * @note
   *  `H % (3600/12) == M / 12`
   * @example
   *  Time: 2024/12/30 14:57:59
   *  - The minute hand swept angle is `3479` in the scale of `3600`
   *  - The hour hand swept angle is `889`.
   *  - The hour hand interval angle is equal to the current hour hand angle minus the hour hand angle at 2 o'clock
   *  - - which is `889` - `300*2` = `289`
   *  - - The formula can be simplified to `H % (3600/12)` where `H` is `889` in this case.
   *  - `H % (3600/12) == M / 12` | `H`:=889; `M`:=`3479` are satisfied with this formula. Therefore it is a valid angle pattern
   */
  {
    VOLATILE uint32_t minute_in_ms     = cmn_utility_mindeg2_ms(pClient->_degree_minute);
    VOLATILE uint32_t delta_hour_in_ms = cmn_utility_hrdeg2_ms((pClient->_degree_hour%(3600/12)));

    VOLATILE uint32_t minute_in_ms_total     = minute_in_ms + pClient->_rem_minute;
    VOLATILE uint32_t delta_hour_in_ms_total = delta_hour_in_ms + pClient->_rem_hour;
    TRACE_DEBUG("ms=%u H_rem=%u M_rem=%u H_deg=%u M_deg=%u h'_ms=%u m_ms=%u H'_ms=%u M_ms=%u", \
      ms,
      pClient->_rem_hour,\
      pClient->_rem_minute,\
      pClient->_degree_hour,\
      pClient->_degree_minute,\
      delta_hour_in_ms,\
      minute_in_ms,\
      delta_hour_in_ms_total,\
      minute_in_ms_total
    );
    ASSERT( minute_in_ms_total == delta_hour_in_ms_total, "Needle Pattern Mismatched");
  }
  
  lv_obj_set_style_transform_angle(pClient->pPinHour, pClient->_degree_hour, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_angle(pClient->pPinMinute, pClient->_degree_minute, LV_PART_MAIN| LV_STATE_DEFAULT);
}

/**
 * @brief UI Clock1 Deinitialization
 * @param [in] pClient - The UI Widget Structure Variable
 * @note Everything in `pClient` will set to `NULL` or `0`
 */
static void ui_clock1_deinit(tAppGuiClockParam *pClient){
  lv_obj_del(pClient->pScreen);
  memset(pClient, 0, sizeof(tAppGuiClockParam));
}


#ifdef __cplusplus
}
#endif


/* ************************************************************************** */
/*                   Static Clock UI Function - clock_nana                    */
/* ************************************************************************** */
#ifndef TEST_ONLY
#ifdef __cplusplus
extern "C"{
#endif
typedef struct{
  lv_obj_t      *ui_nanaeyeclosed;
  lv_obj_t      *ui_nanaeyeopen;
  cmnDateTime_t  time;
}tClockNanaInternalParam;

static void ui_clocknana_init(tAppGuiClockParam *pClient){
  pClient->pScreen = lv_scr_act();
  lv_obj_t *ui_NANA = pClient->pScreen;
  lv_obj_clear_flag( ui_NANA, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_bg_color(ui_NANA, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_NANA, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

  {
    lv_obj_t *ui_MainPanel2 = lv_obj_create(ui_NANA);
    lv_obj_set_width( ui_MainPanel2, 240);
    lv_obj_set_height( ui_MainPanel2, 240);
    lv_obj_set_align( ui_MainPanel2, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_MainPanel2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_MainPanel2, 120, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MainPanel2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_MainPanel2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MainPanel2, lv_color_hex(0xC4C4C4), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_MainPanel2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_MainPanel2, 4, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_MainPanel2, LV_BORDER_SIDE_FULL, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_set_style_outline_width(ui_MainPanel2, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(ui_MainPanel2, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_MainPanel2, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_MainPanel2, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_MainPanel2, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_MainPanel2, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_MainPanel2, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_MainPanel2, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);

    lv_obj_t *ui_Pit12_2 = lv_obj_create(ui_MainPanel2);
    lv_obj_set_width( ui_Pit12_2, 2);
    lv_obj_set_height( ui_Pit12_2, 240);
    lv_obj_set_align( ui_Pit12_2, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_Pit12_2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_bg_color(ui_Pit12_2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_Pit12_2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Pit12_2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_Pit12_2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Pit12_2, 1, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_angle(ui_Pit12_2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_x(ui_Pit12_2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_y(ui_Pit12_2, 120, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_Pit1_2 = lv_obj_create(ui_MainPanel2);
    lv_obj_set_width( ui_Pit1_2, 2);
    lv_obj_set_height( ui_Pit1_2, 240);
    lv_obj_set_align( ui_Pit1_2, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_Pit1_2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_border_color(ui_Pit1_2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_Pit1_2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_angle(ui_Pit1_2, 300, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_x(ui_Pit1_2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_y(ui_Pit1_2, 120, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_Pit2_2 = lv_obj_create(ui_MainPanel2);
    lv_obj_set_width( ui_Pit2_2, 2);
    lv_obj_set_height( ui_Pit2_2, 240);
    lv_obj_set_align( ui_Pit2_2, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_Pit2_2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_border_color(ui_Pit2_2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_Pit2_2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_angle(ui_Pit2_2, 600, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_x(ui_Pit2_2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_y(ui_Pit2_2, 120, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_Pit3_2 = lv_obj_create(ui_MainPanel2);
    lv_obj_set_width( ui_Pit3_2, 2);
    lv_obj_set_height( ui_Pit3_2, 240);
    lv_obj_set_align( ui_Pit3_2, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_Pit3_2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_border_color(ui_Pit3_2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_Pit3_2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_angle(ui_Pit3_2, 900, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_x(ui_Pit3_2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_y(ui_Pit3_2, 120, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_Pit4_2 = lv_obj_create(ui_MainPanel2);
    lv_obj_set_width( ui_Pit4_2, 2);
    lv_obj_set_height( ui_Pit4_2, 240);
    lv_obj_set_align( ui_Pit4_2, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_Pit4_2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_border_color(ui_Pit4_2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_Pit4_2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_angle(ui_Pit4_2, 1200, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_x(ui_Pit4_2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_y(ui_Pit4_2, 120, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_Pit5_2 = lv_obj_create(ui_MainPanel2);
    lv_obj_set_width( ui_Pit5_2, 2);
    lv_obj_set_height( ui_Pit5_2, 240);
    lv_obj_set_align( ui_Pit5_2, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_Pit5_2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_border_color(ui_Pit5_2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_Pit5_2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_angle(ui_Pit5_2, 1500, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_x(ui_Pit5_2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_y(ui_Pit5_2, 120, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_InnerLoop2 = lv_obj_create(ui_MainPanel2);
    lv_obj_set_width( ui_InnerLoop2, 200);
    lv_obj_set_height( ui_InnerLoop2, 200);
    lv_obj_set_align( ui_InnerLoop2, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_InnerLoop2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_InnerLoop2, 180, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_InnerLoop2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_InnerLoop2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_InnerLoop2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_InnerLoop2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  }
  
  tClockNanaInternalParam *p_internal_param = pvPortMalloc(sizeof(tClockNanaInternalParam));
  p_internal_param->ui_nanaeyeclosed = lv_img_create(ui_NANA);
  lv_img_set_src(p_internal_param->ui_nanaeyeclosed, &ui_img_eyes_close_240_png);
  lv_obj_set_width( p_internal_param->ui_nanaeyeclosed, LV_SIZE_CONTENT);  /// 187
  lv_obj_set_height( p_internal_param->ui_nanaeyeclosed, LV_SIZE_CONTENT);   /// 58
  lv_obj_set_x( p_internal_param->ui_nanaeyeclosed, 8 );
  lv_obj_set_y( p_internal_param->ui_nanaeyeclosed, -5 );
  lv_obj_set_align( p_internal_param->ui_nanaeyeclosed, LV_ALIGN_CENTER );
  lv_obj_add_flag( p_internal_param->ui_nanaeyeclosed, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
  lv_obj_clear_flag( p_internal_param->ui_nanaeyeclosed, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  
  p_internal_param->ui_nanaeyeopen = lv_img_create(ui_NANA);
  lv_img_set_src(p_internal_param->ui_nanaeyeopen, &ui_img_eyes_open_240_png);
  lv_obj_set_width( p_internal_param->ui_nanaeyeopen, LV_SIZE_CONTENT);  /// 187
  lv_obj_set_height( p_internal_param->ui_nanaeyeopen, LV_SIZE_CONTENT);   /// 60
  lv_obj_set_x( p_internal_param->ui_nanaeyeopen, 6 );
  lv_obj_set_y( p_internal_param->ui_nanaeyeopen, -7 );
  lv_obj_set_align( p_internal_param->ui_nanaeyeopen, LV_ALIGN_CENTER );
  lv_obj_add_flag( p_internal_param->ui_nanaeyeopen, LV_OBJ_FLAG_ADV_HITTEST | LV_OBJ_FLAG_HIDDEN );   /// Flags
  lv_obj_clear_flag( p_internal_param->ui_nanaeyeopen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

  pClient->p_anything = p_internal_param;

  lv_obj_t *ui_pinMinute = lv_img_create(ui_NANA);
  lv_img_set_src(ui_pinMinute, &ui_img_pin_minute_classic);
  lv_obj_set_width( ui_pinMinute, 16);
  lv_obj_set_height( ui_pinMinute, 96);
  lv_obj_set_x( ui_pinMinute, 0 );
  lv_obj_set_y( ui_pinMinute, -112 );
  lv_obj_set_align( ui_pinMinute, LV_ALIGN_BOTTOM_MID );
  lv_obj_add_flag( ui_pinMinute, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
  lv_obj_clear_flag( ui_pinMinute, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_transform_angle(ui_pinMinute, 900, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_pivot_x(ui_pinMinute, 8, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_pivot_y(ui_pinMinute, 88, LV_PART_MAIN| LV_STATE_DEFAULT);
  pClient->pPinMinute = ui_pinMinute;

  lv_obj_t *ui_pinHour = lv_img_create(ui_NANA);
  lv_img_set_src(ui_pinHour, &ui_img_pin_hour_classic);
  lv_obj_set_width( ui_pinHour, 16);
  lv_obj_set_height( ui_pinHour, 63);
  lv_obj_set_x( ui_pinHour, 0 );
  lv_obj_set_y( ui_pinHour, -112 );
  lv_obj_set_align( ui_pinHour, LV_ALIGN_BOTTOM_MID );
  lv_obj_add_flag( ui_pinHour, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
  lv_obj_clear_flag( ui_pinHour, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_transform_angle(ui_pinHour, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_pivot_x(ui_pinHour, 8, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_pivot_y(ui_pinHour, 55, LV_PART_MAIN| LV_STATE_DEFAULT);
  pClient->pPinHour = ui_pinHour;

}

/**
 * @brief UI Clock1 Set Day Night
 * @param [inout] pClient - The UI Widget Structure Variable
 * @note Update NaNa's eyes
 */
static void ui_clocknana_daynight(tAppGuiClockParam *pClient){
  lv_obj_t *ui_nanaeyeopen   = ((tClockNanaInternalParam*)pClient->p_anything)->ui_nanaeyeopen;
  lv_obj_t *ui_nanaeyeclosed = ((tClockNanaInternalParam*)pClient->p_anything)->ui_nanaeyeclosed;

  if(((tClockNanaInternalParam*)(pClient->p_anything))->time.hour >= 18){
    if(!lv_obj_has_flag(ui_nanaeyeopen, LV_OBJ_FLAG_HIDDEN) || lv_obj_has_flag(ui_nanaeyeclosed, LV_OBJ_FLAG_HIDDEN)){ 
      /* Close Eyes */
      lv_obj_add_flag( ui_nanaeyeopen, LV_OBJ_FLAG_HIDDEN );
      lv_obj_clear_flag( ui_nanaeyeclosed, LV_OBJ_FLAG_HIDDEN);
    }
  }else if(((tClockNanaInternalParam*)(pClient->p_anything))->time.hour >= 8){
    if(lv_obj_has_flag(ui_nanaeyeopen, LV_OBJ_FLAG_HIDDEN) || !lv_obj_has_flag(ui_nanaeyeclosed, LV_OBJ_FLAG_HIDDEN)){
      /* Open Eyes */
      lv_obj_add_flag( ui_nanaeyeclosed, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag( ui_nanaeyeopen, LV_OBJ_FLAG_HIDDEN );
    }
  }
}

static void ui_clocknana_set_time(tAppGuiClockParam *pClient, cmnDateTime_t time){
  ((tClockNanaInternalParam*)(pClient->p_anything))->time = time;
  ui_clock1_set_time(pClient, time);
  ui_clocknana_daynight(pClient);
}

static void ui_clocknana_inc_time(tAppGuiClockParam *pClient, uint32_t ms){
  ui_clock1_inc_time(pClient, ms);
  cmn_utility_timeinc( &(((tClockNanaInternalParam*)(pClient->p_anything))->time), ms);
  ui_clocknana_daynight(pClient);
}

static void ui_clocknana_deinit(tAppGuiClockParam *pClient){
  vPortFree(pClient->p_anything);
  ui_clock1_deinit(pClient);
}

#ifdef __cplusplus
}
#endif
#endif  // #ifndef TEST_ONLY


/* ************************************************************************** */
/*                   Static Clock UI Function - clock_lvvvw                   */
/* ************************************************************************** */
#ifndef TEST_ONLY
#ifdef __cplusplus
extern "C"{
#endif

typedef struct stClockLvvvwInternalParam{
  lv_color_t ruby_color;
  lv_obj_t * ui_ruby;
} tClockLvvvwInternalParam;

static void ui_clocklvvvw_init(tAppGuiClockParam *pClient){
  pClient->pScreen = lv_scr_act();
  pClient->p_anything = pvPortMalloc(sizeof(tClockLvvvwInternalParam));

  lv_obj_clear_flag( pClient->pScreen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_bg_color(pClient->pScreen, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(pClient->pScreen, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_outterloop = lv_obj_create(pClient->pScreen);
  lv_obj_set_width( ui_outterloop, 243);
  lv_obj_set_height( ui_outterloop, 243);
  lv_obj_set_align( ui_outterloop, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_outterloop, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_outterloop, 120, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_outterloop, lv_color_hex(0x4E4D49), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_outterloop, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_color(ui_outterloop, lv_color_hex(0xD6C8BD), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_grad_dir(ui_outterloop, LV_GRAD_DIR_VER, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(ui_outterloop, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_innerloop = lv_obj_create(pClient->pScreen);
  lv_obj_set_width( ui_innerloop, 200);
  lv_obj_set_height( ui_innerloop, 230);
  lv_obj_set_align( ui_innerloop, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_innerloop, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_innerloop, 120, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_innerloop, lv_color_hex(0x3B0909), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_innerloop, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(ui_innerloop, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

  {/* Vivinne Westwood */
    lv_obj_t *ui_saturnbody = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_saturnbody, 56);
    lv_obj_set_height( ui_saturnbody, 56);
    lv_obj_set_x( ui_saturnbody, 0 );
    lv_obj_set_y( ui_saturnbody, 45 );
    lv_obj_set_align( ui_saturnbody, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_saturnbody, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_saturnbody, 90, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_saturnbody, lv_color_hex(0x9A9A9A), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_saturnbody, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_saturnbody, lv_color_hex(0x202020), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_grad_dir(ui_saturnbody, LV_GRAD_DIR_VER, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_saturnbody, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_saturnbody, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_saturnbody, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_saturninterior1 = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_saturninterior1, 72);
    lv_obj_set_height( ui_saturninterior1, 16);
    lv_obj_set_x( ui_saturninterior1, 0 );
    lv_obj_set_y( ui_saturninterior1, 45 );
    lv_obj_set_align( ui_saturninterior1, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_saturninterior1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_saturninterior1, 90, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_saturninterior1, lv_color_hex(0x4B4B4B), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_saturninterior1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_saturninterior1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_saturninterior1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_saturninterior1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_saturninterior2 = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_saturninterior2, 98);
    lv_obj_set_height( ui_saturninterior2, 8);
    lv_obj_set_x( ui_saturninterior2, 0 );
    lv_obj_set_y( ui_saturninterior2, 45 );
    lv_obj_set_align( ui_saturninterior2, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_saturninterior2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_saturninterior2, 90, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_saturninterior2, lv_color_hex(0x6A6A6A), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_saturninterior2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_saturninterior2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_saturninterior2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_saturninterior2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_saturninterior3 = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_saturninterior3, 130);
    lv_obj_set_height( ui_saturninterior3, 4);
    lv_obj_set_x( ui_saturninterior3, 0 );
    lv_obj_set_y( ui_saturninterior3, 45 );
    lv_obj_set_align( ui_saturninterior3, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_saturninterior3, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_saturninterior3, 90, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_saturninterior3, lv_color_hex(0x828282), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_saturninterior3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_saturninterior3, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_saturninterior3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_saturninterior3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_crosshor = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_crosshor, 36);
    lv_obj_set_height( ui_crosshor, 16);
    lv_obj_set_align( ui_crosshor, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_crosshor, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_crosshor, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_crosshor, lv_color_hex(0x6A6A6A), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_crosshor, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_crosshor, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_crosshor, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_crosshor, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_crossver = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_crossver, 16);
    lv_obj_set_height( ui_crossver, 36);
    lv_obj_set_align( ui_crossver, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_crossver, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_crossver, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_crossver, lv_color_hex(0x6A6A6A), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_crossver, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_crossver, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_crossver, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_crossver, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_ruby = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_ruby, 16+1); /*!< An offset of 1 looks better on the screen */
    lv_obj_set_height( ui_ruby, 16);
    lv_obj_set_align( ui_ruby, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_ruby, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_ruby, 8, LV_PART_MAIN| LV_STATE_DEFAULT);
    ((tClockLvvvwInternalParam*)(pClient->p_anything))->ruby_color = lv_color_hex(0xFF0000);
    lv_obj_set_style_bg_color(ui_ruby, ((tClockLvvvwInternalParam*)(pClient->p_anything))->ruby_color, LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_ruby, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_ruby, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_ruby, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_ruby, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
#if 0
    lv_obj_set_style_shadow_color(ui_ruby, ((tClockLvvvwInternalParam*)(pClient->p_anything))->ruby_color, LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_shadow_opa(ui_ruby, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_ruby, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_ruby, 1, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(ui_ruby, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(ui_ruby, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
#endif
    ((tClockLvvvwInternalParam*)(pClient->p_anything))->ui_ruby = ui_ruby;
  }
  
  {/* Digits Icons: 3,9 */
    for(int i=0; i<2; ++i){
      lv_obj_t *ui_digits = lv_img_create(pClient->pScreen);

      lv_img_set_src(ui_digits, &ui_img_lv_flower);
      lv_obj_set_width( ui_digits, LV_SIZE_CONTENT);  /// 32
      lv_obj_set_height( ui_digits, LV_SIZE_CONTENT);   /// 32
      lv_obj_set_x( ui_digits, i==0? -83 : 83 );
      lv_obj_set_y( ui_digits, 0 );
      lv_obj_set_align( ui_digits, LV_ALIGN_CENTER );
      lv_obj_add_flag( ui_digits, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
      lv_obj_clear_flag( ui_digits, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
      lv_obj_set_style_img_recolor(ui_digits, lv_color_hex(0xA78B0B), LV_PART_MAIN| LV_STATE_DEFAULT);
      lv_obj_set_style_img_recolor_opa(ui_digits, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    }
  }

  {/* Digits Icons: 1,2 | 4,5 | 7,8 | 10,11 */
    int8_t tmp[8][2] = {
      { 51,-86}, 
      { 81,-48},
      { 81, 48},
      { 51, 86},
      {-51, 86},
      {-81, 48},
      {-81,-48},
      {-51,-86}
    };

    for(int i=0; i<8; ++i){
      lv_obj_t *ui_digits = lv_img_create(pClient->pScreen);
      lv_img_set_src(ui_digits, &ui_img_lv_leaf);
      lv_obj_set_width( ui_digits, LV_SIZE_CONTENT);  /// 32
      lv_obj_set_height( ui_digits, LV_SIZE_CONTENT);   /// 32
      lv_obj_set_x( ui_digits, tmp[i][0]);
      lv_obj_set_y( ui_digits, tmp[i][1]);
      lv_obj_set_align( ui_digits, LV_ALIGN_CENTER );
      lv_obj_add_flag( ui_digits, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
      lv_obj_clear_flag( ui_digits, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
      lv_img_set_zoom(ui_digits,128);
      lv_obj_set_style_img_recolor(ui_digits, lv_color_hex(0xA78B0B), LV_PART_MAIN| LV_STATE_DEFAULT);
      lv_obj_set_style_img_recolor_opa(ui_digits, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    }
  }

  {/* Digits Icons: 6 */
    lv_obj_t *ui_digits = lv_img_create(pClient->pScreen);
    lv_img_set_src(ui_digits, &ui_img_lv_spad);
    lv_obj_set_width( ui_digits, LV_SIZE_CONTENT);  /// 32
    lv_obj_set_height( ui_digits, LV_SIZE_CONTENT);   /// 32
    lv_obj_set_x( ui_digits, 0 );
    lv_obj_set_y( ui_digits, 97 );
    lv_obj_set_align( ui_digits, LV_ALIGN_CENTER );
    lv_obj_add_flag( ui_digits, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
    lv_obj_clear_flag( ui_digits, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_img_recolor(ui_digits, lv_color_hex(0xA78B0B), LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_digits, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  }

  {/* Digits Icons: 12 */
    lv_obj_t *ui_digits = lv_img_create(pClient->pScreen);
    lv_img_set_src(ui_digits, &ui_img_12roman_240_png);
    lv_obj_set_width( ui_digits, LV_SIZE_CONTENT);  /// 52
    lv_obj_set_height( ui_digits, LV_SIZE_CONTENT);   /// 64
    lv_obj_set_x( ui_digits, -2 );
    lv_obj_set_y( ui_digits, -73 );
    lv_obj_set_align( ui_digits, LV_ALIGN_CENTER );
    lv_obj_add_flag( ui_digits, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
    lv_obj_clear_flag( ui_digits, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  }

  {/* Needle: Hour / Minute */
    lv_obj_t *ui_pinHour = lv_img_create(pClient->pScreen);
    lv_img_set_src(ui_pinHour, &ui_img_pin_hour_classic);
    lv_obj_set_width( ui_pinHour, 16);
    lv_obj_set_height( ui_pinHour, 63);
    lv_obj_set_x( ui_pinHour, 0 );
    lv_obj_set_y( ui_pinHour, -112 );
    lv_obj_set_align( ui_pinHour, LV_ALIGN_BOTTOM_MID );
    lv_obj_add_flag( ui_pinHour, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
    lv_obj_clear_flag( ui_pinHour, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_img_recolor(ui_pinHour, lv_color_hex(0xE5E17B), LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_pinHour, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_angle(ui_pinHour, 1234, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_x(ui_pinHour, 8, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_y(ui_pinHour, 55, LV_PART_MAIN| LV_STATE_DEFAULT);
    pClient->pPinHour = ui_pinHour;

    lv_obj_t *ui_pinMinute = lv_img_create(pClient->pScreen);
    lv_img_set_src(ui_pinMinute, &ui_img_pin_minute_classic);
    lv_obj_set_width( ui_pinMinute, 16);
    lv_obj_set_height( ui_pinMinute, 96);
    lv_obj_set_x( ui_pinMinute, 0 );
    lv_obj_set_y( ui_pinMinute, -112 );
    lv_obj_set_align( ui_pinMinute, LV_ALIGN_BOTTOM_MID );
    lv_obj_add_flag( ui_pinMinute, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
    lv_obj_clear_flag( ui_pinMinute, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_img_recolor(ui_pinMinute, lv_color_hex(0xE5E17B), LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_pinMinute, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_angle(ui_pinMinute, 2400, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_x(ui_pinMinute, 8, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_y(ui_pinMinute, 88, LV_PART_MAIN| LV_STATE_DEFAULT);
    pClient->pPinMinute = ui_pinMinute;
  }
}

static void ui_clocklvvvw_set_time(tAppGuiClockParam *pClient, cmnDateTime_t time){
  ui_clock1_set_time(pClient, time);

  /**
   * @todo: Screen shows a glich when moving to init()
   */
#if 1
  lv_obj_t *ui_ruby = ((tClockLvvvwInternalParam*)(pClient->p_anything))->ui_ruby;
  lv_obj_set_style_shadow_color(ui_ruby, ((tClockLvvvwInternalParam*)(pClient->p_anything))->ruby_color, LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_shadow_opa(ui_ruby, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_width(ui_ruby, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_spread(ui_ruby, 1, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_ofs_x(ui_ruby, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_ofs_y(ui_ruby, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
#endif
}

static void ui_clocklvvvw_inc_time(tAppGuiClockParam *pClient, uint32_t ms){
  ui_clock1_inc_time(pClient, ms);
}

static void ui_clocklvvvw_deinit(tAppGuiClockParam *pClient){
  vPortFree(pClient->p_anything);
  ui_clock1_deinit(pClient);
}

#ifdef __cplusplus
}
#endif
#endif  // #ifndef TEST_ONLY



/* ************************************************************************** */
/*                          Public Clock UI Function                          */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif


/**
 * @brief Switch the Clock GUI
 * @param [in] x  - The GUI Enumeration. `0` means deactivating the clock UI.
 * @warning
 *  This function assumes the `deinit()` is NULL when clock ui is deactivated, vice versa.
 */
void app_gui_switch( AppGuiClockEnum_t x){
  if(NULL!=metope.app.clock.gui.deinit){
    metope.app.clock.gui.deinit( &metope.app.clock.gui.param );
  }
  switch(x){
    case kAppGuiClock_None:{
      /**
       * @todo: Can't be NULL. Too dangerous.
       */
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
      ASSERT( false, "Unknown clock theme");
      break;
    }
#ifndef TEST_ONLY
    /**
     * @note: The rest of UI is ONLY for main program because of the memory usage
     */
    case kAppGuiClock_NANA:{
      metope.app.clock.gui.init     = ui_clocknana_init;
      metope.app.clock.gui.set_time = ui_clocknana_set_time;
      metope.app.clock.gui.inc_time = ui_clocknana_inc_time;
      metope.app.clock.gui.deinit   = ui_clocknana_deinit;
      break;
    }
    case kAppGuiClock_LVVVW:{
      metope.app.clock.gui.init     = ui_clocklvvvw_init;
      metope.app.clock.gui.set_time = ui_clocklvvvw_set_time;
      metope.app.clock.gui.inc_time = ui_clocklvvvw_inc_time;
      metope.app.clock.gui.deinit   = ui_clocklvvvw_deinit;
      break;
    }

#endif
  }

  metope.app.clock.gui.init( &metope.app.clock.gui.param );
}

/**
 * @brief Module the clock parameters in case of overflow
 * @param [inout] pClient - The UI Widget Structure Variable
 * @bug NOT threadsafe !!!
 */
void app_gui_update_modulo(tAppGuiClockParam *pClient) RTOSIDLE{
  pClient->_degree_hour   %= 3600;
  pClient->_degree_minute %= 3600;
}



/**
 * @param [in] param  - will cast to `tAppClock`
 */
void app_clock_gui_main(void *param) RTOSTHREAD{
  tAppClock *parsed_param = (tAppClock *)param;

  portTICK_TYPE_ENTER_CRITICAL();
  app_gui_switch(parsed_param->style);
  portTICK_TYPE_EXIT_CRITICAL();
  parsed_param->time = bsp_rtc_get_time();
  parsed_param->gui.set_time( &parsed_param->gui.param, parsed_param->time);
  
  /**
   * @todo: Need to update the time from RTC
   */
  TickType_t old_tick = xTaskGetTickCount();
  while(1){
    /**
     * @note
     *  Control Part:
     *    1) Perpare the updated time.
     *    2) Calculate the increased ms.
     */
    TickType_t tmp = xTaskGetTickCount();
    TickType_t ms_delta = tmp - old_tick;
    old_tick = tmp;
    cmn_utility_timeinc( &parsed_param->time, ms_delta); /** @bug */
    

    EventBits_t uxBits = xEventGroupWaitBits( metope.app.rtos.event._handle, CMN_EVENT_UPDATE_RTC, pdTRUE, pdFALSE, 64);

    portTICK_TYPE_ENTER_CRITICAL();
    if(uxBits & CMN_EVENT_UPDATE_RTC){
      parsed_param->time = bsp_rtc_get_time();
      parsed_param->gui.set_time( &parsed_param->gui.param, parsed_param->time);
      /**
       * @bug 
       *  LVGL can not finish a correct partial refreash after a big needle angle change
       */
      lv_obj_invalidate(parsed_param->gui.param.pScreen);
    }else{
      /**
       * @note
       *  View Part:
       *    1) Update the increased ms.
       */
      parsed_param->gui.inc_time( &parsed_param->gui.param, ms_delta);
    }
    portTICK_TYPE_EXIT_CRITICAL();
  }
}


#ifdef __cplusplus
}
#endif
