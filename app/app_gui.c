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
#include "FreeRTOS.h"
#include "timers.h"
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
#define MAX_CLOCK_DIFF_SEC             (3)
#define DEFAULT_IDLE_TASK_PERIOD       (3000) // ms
#define DEFAULT_CLOCK_REFREASH_PERIOD  (64)   // ticks



/* ************************************************************************** */
/*                                Private Tags                                */
/* ************************************************************************** */
#define APP_CLOCK_API
#define APP_CLOCK_GLOBAL



/* ************************************************************************** */
/*                    Private Clock UI Function Prototype                     */
/* ************************************************************************** */
static void         app_clock_gui_switch         (AppGuiClockEnum_t x);
static void         app_clock_idle_timer_callback(xTimerHandle xTimer);
static xTimerHandle app_clock_idle_timer_regist  (void);
static void         app_clock_idle_timer_unregist(xTimerHandle xTimer);

/**
 * @brief Private time check
 * @note If time is expired, a set time operation will be triggered.
 * @note Tolerance is aligned with `MAX_CLOCK_DIFF_SEC`
 * @param [in] rtc_time - Real Time Clock Time
 * @param [in] clk_time - UI Clock to be checked
 * @return Return `true` if time is expired
 * @addtogroup NotThreadSafe
 */
static cmnBoolean_t is_time_expired(cmnDateTime_t rtc_time, cmnDateTime_t clk_time){
  int32_t diff = cmn_utility_timediff(rtc_time, clk_time);

  if( CMN_ABS(diff) > MAX_CLOCK_DIFF_SEC ){
    TRACE_DEBUG("RTC=%u/%u/%u %u:%u:%u CLK=%u/%u/%u %u:%u:%u",\
      rtc_time.year+2024,                                     \
      rtc_time.month,                                         \
      rtc_time.day,                                           \
      rtc_time.hour,                                          \
      rtc_time.minute,                                        \
      rtc_time.second,                                        \
      clk_time.year+2024,                                     \
      clk_time.month,                                         \
      clk_time.day,                                           \
      clk_time.hour,                                          \
      clk_time.minute,                                        \
      clk_time.second                                         \
    );
    return true;
  }else{
    return false;
  }
}




/* ************************************************************************** */
/*                Abstract [Analog Clock] UI Common Parameters                */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

/**
 * @note
 *  Abstract UI Internal Cache for Analog Clock Style
 */
typedef struct tAnalogClockInternalParam{
  uint16_t _degree_hour;
  uint16_t _degree_minute;
  uint16_t _rem_hour;
  uint16_t _rem_minute;
  uint32_t _rem_microsecond;
} tAnalogClockInternalParam;

static void analogclk_set_time(tAppGuiClockParam *pClient, tAnalogClockInternalParam *params, cmnDateTime_t time);
static void analogclk_inc_time(tAppGuiClockParam *pClient, tAnalogClockInternalParam *params, uint32_t ms);
static void analogclk_idle    (tAppGuiClockParam *pClient, tAnalogClockInternalParam *params);

/**
 * @brief Analog Clock Set Time Function
 * @param [inout] pClient - The UI Widget Structure Variable
 * @param [inout] params  - The Abstract Analog Clock Parameters
 * @param [in]    time    - Absolute Real Time "YYYY/MM/DD HH:MM:SS"
 * @note Update needle angle
 */
static void analogclk_set_time(tAppGuiClockParam *pClient, tAnalogClockInternalParam *params, cmnDateTime_t time){
  cmn_utility_angleset( &params->_rem_hour, &params->_rem_minute, NULL, &params->_degree_hour, &params->_degree_minute, NULL, &time);

  pClient->time            = time;
  params->_rem_microsecond = 0;
  
  lv_obj_set_style_transform_angle(pClient->pPinHour, params->_degree_hour, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_angle(pClient->pPinMinute, params->_degree_minute, LV_PART_MAIN| LV_STATE_DEFAULT);
}

/**
 * @brief Analog Clock Inc Time Function
 * @param [inout] pClient - The UI Widget Structure Variable
 * @param [inout] params  - The Abstract Analog Clock Parameters
 * @param [in]    ms      - Escaped Microseconds
 * @note Update needle angle
 * @addtogroup NotThreadSafe
 */
static void analogclk_inc_time(tAppGuiClockParam *pClient, tAnalogClockInternalParam *params, uint32_t ms){
  uint16_t hour_inc, minute_inc;
  cmn_utility_angleinc( &params->_rem_hour, &params->_rem_minute, NULL, &hour_inc, &minute_inc, NULL, ms);
  
  params->_degree_hour += hour_inc;
  params->_degree_minute += minute_inc;

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
    VOLATILE uint32_t minute_in_ms     = cmn_utility_mindeg2_ms(params->_degree_minute);
    VOLATILE uint32_t delta_hour_in_ms = cmn_utility_hrdeg2_ms((params->_degree_hour%(3600/12)));

    VOLATILE uint32_t minute_in_ms_total     = minute_in_ms + params->_rem_minute;
    VOLATILE uint32_t delta_hour_in_ms_total = delta_hour_in_ms + params->_rem_hour;
    TRACE_DUMMY("ms=%u H_rem=%u M_rem=%u H_deg=%u M_deg=%u h'_ms=%u m_ms=%u H'_ms=%u M_ms=%u",\
      ms,                                                                                     \
      params->_rem_hour,                                                                      \
      params->_rem_minute,                                                                    \
      params->_degree_hour,                                                                   \
      params->_degree_minute,                                                                 \
      delta_hour_in_ms,                                                                       \
      minute_in_ms,                                                                           \
      delta_hour_in_ms_total,                                                                 \
      minute_in_ms_total                                                                      \
    );
    ASSERT(                                                  \
      (minute_in_ms_total == delta_hour_in_ms_total)         \
      ||                                                     \
      (minute_in_ms_total%3600000 == delta_hour_in_ms_total),\
      "Needle Pattern Mismatched"                            \
    );
  }

  cmn_utility_timeinc( &params->_rem_microsecond, &pClient->time, ms);
  {
    cmnDateTime_t time = pClient->time;
    TRACE_DUMMY("After %u ms, time => %u/%u/%u %u:%u:%u rem_ms=%u", ms, time.year+2024, time.month, time.day, time.hour, time.minute, time.second, params->_rem_microsecond);
  }
  
  lv_obj_set_style_transform_angle(pClient->pPinHour, params->_degree_hour, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_transform_angle(pClient->pPinMinute, params->_degree_minute, LV_PART_MAIN| LV_STATE_DEFAULT);
}

/**
 * @addtogroup NotThreadSafe
 */
static void analogclk_idle(tAppGuiClockParam *pClient, tAnalogClockInternalParam *params){
  if(params->_degree_hour > 3600){
    params->_degree_hour %= 3600;
  }
  if(params->_degree_minute > 3600){
    params->_degree_minute %= 3600;
  }
}

#ifdef __cplusplus
}
#endif

/* ************************************************************************** */
/*                     Private Clock UI Function - clock1                     */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

static void ui_clock1_init    (tAppGuiClockParam *pClient);
static void ui_clock1_set_time(tAppGuiClockParam *pClient, cmnDateTime_t time);
static void ui_clock1_inc_time(tAppGuiClockParam *pClient, uint32_t ms);
static void ui_clock1_idle    (tAppGuiClockParam *pClient);
static void ui_clock1_deinit  (tAppGuiClockParam *pClient);

typedef struct{
  tAnalogClockInternalParam  analog_clk;
}tClock1InternalParam;

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
  tClock1InternalParam *pClientPrivateParams = (tClock1InternalParam *)pClient->customized.p_anything;
  analogclk_set_time( pClient, &pClientPrivateParams->analog_clk, time);
}

/**
 * @brief UI Clock1 Inc Time (ms)
 * @param [inout] pClient - The UI Widget Structure Variable
 * @note Update needle angle
 */
static void ui_clock1_inc_time(tAppGuiClockParam *pClient, uint32_t ms){
  tClock1InternalParam *pClientPrivateParams = (tClock1InternalParam *)pClient->customized.p_anything;
  analogclk_inc_time( pClient, &pClientPrivateParams->analog_clk, ms);
}

static void ui_clock1_idle(tAppGuiClockParam *pClient){
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ASSERT(ret==pdTRUE, "Data was NOT obtained");

  tClock1InternalParam *pClientPrivateParams = (tClock1InternalParam *)pClient->customized.p_anything;
  analogclk_idle(pClient, &pClientPrivateParams->analog_clk);
  xSemaphoreGive(pClient->customized._semphr);
}

/**
 * @brief UI Clock1 Deinitialization
 * @param [in] pClient - The UI Widget Structure Variable
 * @note Everything in `pClient` will set to `NULL` or `0`
 */
static void ui_clock1_deinit(tAppGuiClockParam *pClient){
  lv_obj_del(pClient->pScreen);
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ASSERT(ret==pdTRUE, "Data was NOT obtained");

  if(NULL!=pClient->customized.p_anything){
    vPortFree(pClient->customized.p_anything);
    pClient->customized.p_anything = NULL;
  }
  xSemaphoreGive(pClient->customized._semphr);
  /**
   * @warning
   *  Directly delete the mutex may cause concurrency bugs.
   * @note
   *  FreeRTOS does not delete a semaphore that has tasks blocked on it 
   *  (tasks that are in the Blocked state waiting for the semaphore to 
   *  become available).
   */
  vSemaphoreDelete(pClient->customized._semphr);
  memset(pClient, 0, sizeof(tAppGuiClockParam));
}


#ifdef __cplusplus
}
#endif


/* ************************************************************************** */
/*                   Private Clock UI Function - clock_nana                   */
/* ************************************************************************** */
#ifndef TEST_ONLY
#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
  lv_obj_t                  *ui_nanaeyeclosed;
  lv_obj_t                  *ui_nanaeyeopen;
  tAnalogClockInternalParam  analog_clk;
}tClockNanaInternalParam;

static void ui_clocknana_init    (tAppGuiClockParam *pClient)                     APP_CLOCK_API;
static void ui_clocknana_set_time(tAppGuiClockParam *pClient, cmnDateTime_t time) APP_CLOCK_API;
static void ui_clocknana_inc_time(tAppGuiClockParam *pClient, uint32_t ms)        APP_CLOCK_API;
static void ui_clocknana_daynight(tAppGuiClockParam *pClient);
static void ui_clocknana_idle    (tAppGuiClockParam *pClient)                     APP_CLOCK_API;
static void ui_clocknana_deinit  (tAppGuiClockParam *pClient)                     APP_CLOCK_API;

/**
 * @brief UI ClockNaNa Initialization
 * @param [inout] pClient - The UI Widget Structure Variable
 * @addtogroup ThreadSafe
 */
static void ui_clocknana_init(tAppGuiClockParam *pClient){
  pClient->customized._semphr = xSemaphoreCreateMutex();
  ASSERT(pClient->customized._semphr, "Mutex was NOT created");
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  
  ///////////////////////////////////////////////////////////////
  /////////////////////// Safe Zone Start ///////////////////////
  ASSERT(ret==pdTRUE, "Data was NOT obtained");

  lv_obj_t *ui_NANA = lv_scr_act();
  {
    lv_obj_clear_flag( ui_NANA, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_bg_color(ui_NANA, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_NANA, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  }
  pClient->pScreen = ui_NANA;

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
  
  tClockNanaInternalParam *pClientPrivateParams = (tClockNanaInternalParam *)pvPortMalloc(sizeof(tClockNanaInternalParam));
  {
    lv_obj_t* ui_nanaeyeclosed = lv_img_create(ui_NANA);
    lv_img_set_src( ui_nanaeyeclosed, &ui_img_eyes_close_240_png);
    lv_obj_set_width( ui_nanaeyeclosed, LV_SIZE_CONTENT);  /// 187
    lv_obj_set_height( ui_nanaeyeclosed, LV_SIZE_CONTENT);   /// 58
    lv_obj_set_x( ui_nanaeyeclosed, 8 );
    lv_obj_set_y( ui_nanaeyeclosed, -5 );
    lv_obj_set_align( ui_nanaeyeclosed, LV_ALIGN_CENTER );
    lv_obj_add_flag( ui_nanaeyeclosed, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
    lv_obj_clear_flag( ui_nanaeyeclosed, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    pClientPrivateParams->ui_nanaeyeclosed = ui_nanaeyeclosed;
  }
  
  {
    lv_obj_t* ui_nanaeyeopen = lv_img_create(ui_NANA);
    lv_img_set_src( ui_nanaeyeopen, &ui_img_eyes_open_240_png);
    lv_obj_set_width( ui_nanaeyeopen, LV_SIZE_CONTENT);  /// 187
    lv_obj_set_height( ui_nanaeyeopen, LV_SIZE_CONTENT);   /// 60
    lv_obj_set_x( ui_nanaeyeopen, 6 );
    lv_obj_set_y( ui_nanaeyeopen, -7 );
    lv_obj_set_align( ui_nanaeyeopen, LV_ALIGN_CENTER );
    lv_obj_add_flag( ui_nanaeyeopen, LV_OBJ_FLAG_ADV_HITTEST | LV_OBJ_FLAG_HIDDEN );   /// Flags
    lv_obj_clear_flag( ui_nanaeyeopen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    pClientPrivateParams->ui_nanaeyeopen = ui_nanaeyeopen;
  }
  pClient->customized.p_anything = pClientPrivateParams;

  {
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
  }

  {
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

  pClient->_idle_task_timer = app_clock_idle_timer_regist();
  xTimerStart(pClient->_idle_task_timer, 0);
  //////////////////////// Safe Zone End ////////////////////////
  ///////////////////////////////////////////////////////////////
  ret = xSemaphoreGive(pClient->customized._semphr);
  ASSERT(ret==pdTRUE, "Data was NOT released");
}

/**
 * @brief UI ClockNaNa Set Day Night
 * @param [inout] pClient - The UI Widget Structure Variable
 * @note Update NaNa's eyes
 * @addtogroup NotThreadSafe
 */
static void ui_clocknana_daynight(tAppGuiClockParam *pClient){
  tClockNanaInternalParam *pClientPrivateParams = (tClockNanaInternalParam *)pClient->customized.p_anything;
  lv_obj_t *ui_nanaeyeopen   = pClientPrivateParams->ui_nanaeyeopen;
  lv_obj_t *ui_nanaeyeclosed = pClientPrivateParams->ui_nanaeyeclosed;

  if(pClient->time.hour >= 18){
    if(!lv_obj_has_flag(ui_nanaeyeopen, LV_OBJ_FLAG_HIDDEN) || lv_obj_has_flag(ui_nanaeyeclosed, LV_OBJ_FLAG_HIDDEN)){ 
      /* Close Eyes */
      lv_obj_add_flag( ui_nanaeyeopen, LV_OBJ_FLAG_HIDDEN );
      lv_obj_clear_flag( ui_nanaeyeclosed, LV_OBJ_FLAG_HIDDEN);
    }
  }else if(pClient->time.hour >= 8){
    if(lv_obj_has_flag(ui_nanaeyeopen, LV_OBJ_FLAG_HIDDEN) || !lv_obj_has_flag(ui_nanaeyeclosed, LV_OBJ_FLAG_HIDDEN)){
      /* Open Eyes */
      lv_obj_add_flag( ui_nanaeyeclosed, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag( ui_nanaeyeopen, LV_OBJ_FLAG_HIDDEN);
    }
  }
}

/**
 * @brief UI ClockNaNa Set Time
 * @param [inout] pClient - The UI Widget Structure Variable
 * @param [in]    time    - RTC Time
 * @addtogroup ThreadSafe
 */
static void ui_clocknana_set_time(tAppGuiClockParam *pClient, cmnDateTime_t time){
  tClockNanaInternalParam *pClientPrivateParams = (tClockNanaInternalParam *)pClient->customized.p_anything;
  
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ///////////////////////////////////////////////////////////////
  /////////////////////// Safe Zone Start ///////////////////////
  ASSERT(ret==pdTRUE, "Data was NOT obtained");

  analogclk_set_time( pClient, &pClientPrivateParams->analog_clk, time);
  ui_clocknana_daynight(pClient);
  //////////////////////// Safe Zone End ////////////////////////
  ///////////////////////////////////////////////////////////////
  xSemaphoreGive(pClient->customized._semphr);
}

/**
 * @brief UI ClockNaNa Increase Time
 * @param [inout] pClient - The UI Widget Structure Variable
 * @param [in]    ms      - Escaped Microseconds
 * @addtogroup ThreadSafe
 */
static void ui_clocknana_inc_time(tAppGuiClockParam *pClient, uint32_t ms){
  tClockNanaInternalParam *pClientPrivateParams = (tClockNanaInternalParam *)pClient->customized.p_anything;
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ///////////////////////////////////////////////////////////////
  /////////////////////// Safe Zone Start ///////////////////////
  ASSERT(ret==pdTRUE, "Data was NOT obtained");
  
  analogclk_inc_time( pClient, &pClientPrivateParams->analog_clk, ms);
  ui_clocknana_daynight(pClient);
  //////////////////////// Safe Zone End ////////////////////////
  ///////////////////////////////////////////////////////////////
  xSemaphoreGive(pClient->customized._semphr);
}

/**
 * @brief UI ClockNaNa Idle Execution
 * @note Mathmatical Modulo / Time Adjustment
 * @param [inout] pClient - The UI Widget Structure Variable
 * @addtogroup ThreadSafe
 */
static void ui_clocknana_idle(tAppGuiClockParam *pClient){
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ///////////////////////////////////////////////////////////////
  /////////////////////// Safe Zone Start ///////////////////////
  ASSERT(ret==pdTRUE, "Data was NOT obtained");
  tClockNanaInternalParam *pClientPrivateParams = (tClockNanaInternalParam *)pClient->customized.p_anything;

  analogclk_idle( pClient, &pClientPrivateParams->analog_clk);

  /**
   * @note: Store to the temperary variable to avoid dead lock
   */
  cmnDateTime_t clk_time = pClient->time;

  //////////////////////// Safe Zone End ////////////////////////
  ///////////////////////////////////////////////////////////////
  xSemaphoreGive(pClient->customized._semphr);
  
  vTaskSuspendAll();
  ///////////////////////////////////////////////////////////////
  /////////////////////// Safe Zone Start ///////////////////////
  cmnDateTime_t rtc_time = bsp_rtc_get_time();
  //////////////////////// Safe Zone End ////////////////////////
  ///////////////////////////////////////////////////////////////
  xTaskResumeAll();

  cmnBoolean_t is_rtc_being_updated = CMN_EVENT_UPDATE_RTC & xEventGroupGetBits(metope.app.rtos.event._handle);
  if( !is_rtc_being_updated && is_time_expired( rtc_time, clk_time)){
    /**
     * @todo: Check the return type
     */
    TRACE_WARNING("Time offset reaches the max allowed value.");
    xEventGroupSetBits( metope.app.rtos.event._handle, CMN_EVENT_UPDATE_RTC);
  }
}

/**
 * @brief UI ClockNaNa Deinitialization
 * @param [inout] pClient - The UI Widget Structure Variable
 * @addtogroup ThreadSafe
 */
static void ui_clocknana_deinit(tAppGuiClockParam *pClient){
  SemaphoreHandle_t _semphr = pClient->customized._semphr;
  BaseType_t            ret = xSemaphoreTake( _semphr, portMAX_DELAY);
  ASSERT(ret==pdTRUE, "Data was NOT obtained");

  ///////////////////////////////////////////////////////////////
  /////////////////////// Safe Zone Start ///////////////////////
  lv_obj_del(pClient->pScreen);
  {
    tClockNanaInternalParam *pClientPrivateParams = (tClockNanaInternalParam *)pClient->customized.p_anything;
    memset(pClientPrivateParams, 0, sizeof(tClockNanaInternalParam));
    vPortFree(pClientPrivateParams);
    pClient->customized.p_anything = NULL;
  }
  
  app_clock_idle_timer_unregist(pClient->_idle_task_timer);

  memset(pClient, 0, sizeof(tAppGuiClockParam));
  //////////////////////// Safe Zone End ////////////////////////
  ///////////////////////////////////////////////////////////////
  /**
   * @warning
   *  Directly delete the mutex may cause concurrency bugs.
   * @note
   *  FreeRTOS does not delete a semaphore that has tasks blocked on it 
   *  (tasks that are in the Blocked state waiting for the semaphore to 
   *  become available).
   */
  xSemaphoreGive(_semphr);
  vSemaphoreDelete(_semphr);
}

#ifdef __cplusplus
}
#endif
#endif  // #ifndef TEST_ONLY


/* ************************************************************************** */
/*                  Private Clock UI Function - clock_lvvvw                   */
/* ************************************************************************** */
#ifndef TEST_ONLY
#ifdef __cplusplus
extern "C"{
#endif

static void ui_clocklvvvw_init    (tAppGuiClockParam *pClient);
static void ui_clocklvvvw_set_time(tAppGuiClockParam *pClient, cmnDateTime_t time);
static void ui_clocklvvvw_inc_time(tAppGuiClockParam *pClient, uint32_t ms);
static void ui_clocklvvvw_idle    (tAppGuiClockParam *pClient);
static void ui_clocklvvvw_deinit  (tAppGuiClockParam *pClient);

typedef struct stClockLvvvwInternalParam{
  lv_color_t                 ruby_color;
  lv_obj_t                  *ui_ruby;
  tAnalogClockInternalParam  analog_clk;
} tClockLvvvwInternalParam;

static void ui_clocklvvvw_init(tAppGuiClockParam *pClient){
  pClient->pScreen = lv_scr_act();
  pClient->customized._semphr = xSemaphoreCreateMutex();
  ASSERT(pClient->customized._semphr, "Mutex was NOT created");
  
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ASSERT(ret==pdTRUE, "Data was NOT obtained");
  tClockLvvvwInternalParam *pClientPrivateParams = (tClockLvvvwInternalParam *)pvPortMalloc(sizeof(tClockLvvvwInternalParam));

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
    pClientPrivateParams->ruby_color = lv_color_hex(0xFF0000);
    lv_obj_set_style_bg_color(ui_ruby, pClientPrivateParams->ruby_color, LV_PART_MAIN | LV_STATE_DEFAULT );
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
    pClientPrivateParams->ui_ruby = ui_ruby;
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

  pClient->customized.p_anything = pClientPrivateParams;
  ret = xSemaphoreGive(pClient->customized._semphr);
  ASSERT(ret==pdTRUE, "Data was NOT released");
}

static void ui_clocklvvvw_set_time(tAppGuiClockParam *pClient, cmnDateTime_t time){
  tClockLvvvwInternalParam *pClientPrivateParams = (tClockLvvvwInternalParam *)pClient->customized.p_anything;
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ASSERT(ret==pdTRUE, "Data was NOT obtained");

  analogclk_set_time( pClient, &pClientPrivateParams->analog_clk, time);

  /**
   * @todo: Screen shows a glich when moving to init()
   */
#if 1
  lv_obj_t *ui_ruby = pClientPrivateParams->ui_ruby;
  lv_obj_set_style_shadow_color(ui_ruby, pClientPrivateParams->ruby_color, LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_shadow_opa(ui_ruby, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_width(ui_ruby, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_spread(ui_ruby, 1, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_ofs_x(ui_ruby, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_ofs_y(ui_ruby, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
#endif

  xSemaphoreGive(pClient->customized._semphr);
}

static void ui_clocklvvvw_inc_time(tAppGuiClockParam *pClient, uint32_t ms){
  tClockLvvvwInternalParam *pClientPrivateParams = (tClockLvvvwInternalParam *)pClient->customized.p_anything;
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ASSERT(ret==pdTRUE, "Data was NOT obtained");

  analogclk_inc_time( pClient, &pClientPrivateParams->analog_clk, ms);
  
  xSemaphoreGive(pClient->customized._semphr);
}

static void ui_clocklvvvw_idle(tAppGuiClockParam *pClient){
  tClockLvvvwInternalParam *pClientPrivateParams = (tClockLvvvwInternalParam *)pClient->customized.p_anything;
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ASSERT(ret==pdTRUE, "Data was NOT obtained");

  analogclk_idle( pClient, &pClientPrivateParams->analog_clk);
  
  xSemaphoreGive(pClient->customized._semphr);
}

static void ui_clocklvvvw_deinit(tAppGuiClockParam *pClient){
  ui_clock1_deinit(pClient);
}

#ifdef __cplusplus
}
#endif
#endif  // #ifndef TEST_ONLY




#ifdef __cplusplus
extern "C"{
#endif

/* ************************************************************************** */
/*                         Private Clock UI Function                          */
/* ************************************************************************** */
/**
 * @brief Switch the Clock GUI
 * @param [in] x  - The GUI Enumeration. `0` means deactivating the clock UI.
 * @warning
 *  This function assumes the `deinit()` is NULL when clock ui is deactivated, vice versa.
 */
static void app_clock_gui_switch( AppGuiClockEnum_t x){
  if(NULL!=metope.app.clock.gui.deinit){
    metope.app.clock.gui.deinit( &metope.app.clock.gui.param );
  }
  vTaskSuspendAll();
  /////////////////////// Safe Zone Start ///////////////////////
  switch(x){
    case kAppGuiClock_None:{
      /**
       * @todo: Can't be NULL. Too dangerous.
       */
      metope.app.clock.gui.init     = NULL;
      metope.app.clock.gui.set_time = NULL;
      metope.app.clock.gui.inc_time = NULL;
      metope.app.clock.gui.idle     = NULL;
      metope.app.clock.gui.deinit   = NULL;
      break;
    }
    case kAppGuiClock_Clock1:{
      metope.app.clock.gui.init     = ui_clock1_init;
      metope.app.clock.gui.set_time = ui_clock1_set_time;
      metope.app.clock.gui.inc_time = ui_clock1_inc_time;
      metope.app.clock.gui.idle     = ui_clock1_idle;
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
      metope.app.clock.gui.idle     = ui_clocknana_idle;
      metope.app.clock.gui.deinit   = ui_clocknana_deinit;
      break;
    }
    case kAppGuiClock_LVVVW:{
      metope.app.clock.gui.init     = ui_clocklvvvw_init;
      metope.app.clock.gui.set_time = ui_clocklvvvw_set_time;
      metope.app.clock.gui.inc_time = ui_clocklvvvw_inc_time;
      metope.app.clock.gui.idle     = ui_clocklvvvw_idle;
      metope.app.clock.gui.deinit   = ui_clocklvvvw_deinit;
      break;
    }

#endif
  }

  //////////////////////// Safe Zone End ////////////////////////
  xTaskResumeAll();

  metope.app.clock.gui.init( &metope.app.clock.gui.param );
}

/**
 * @brief Idle Timer Callback Function
 * @note 
 *  If an idle program was configured, the software timer for clock application will be activated
 *  to periodically run idle programs without intervening the main thread.
 * @note 
 *  The period of the software timer is set by `DEFAULT_IDLE_TASK_PERIOD`. This function will ONLY
 *  set the bit map to notify the idle program was ready to go.
 *  The idle program should unset this bit once completed
 * 
 * @param [in] xTimer - FreeRTOS Software Timer Handle
 * @addtogroup FreeRTOS
 */
static void app_clock_idle_timer_callback(xTimerHandle xTimer){
  if(xTimer && !xTimerIsTimerActive(xTimer)){
    metope.app.rtos.task.bitmap_idle.clock = 1;
  }
}

/**
 * @brief Idle Timer Registration
 * @note Initialize the software timer for idle task
 * 
 * @addtogroup FreeRTOS
 */
static xTimerHandle app_clock_idle_timer_regist(void){
  xTimerHandle _idle_task_timer = xTimerCreate( \
    "app_clock_idle_timer",
    pdMS_TO_TICKS(DEFAULT_IDLE_TASK_PERIOD),
    pdFALSE,
    app_clock_idle_timer_regist,
    app_clock_idle_timer_callback);
  
  ASSERT( _idle_task_timer, "Timer is NOT created");

  return _idle_task_timer;
}

/**
 * @brief Idle Timer DeRegistration
 * @note Delete the software timer for idle task
 * @param [in] xTimer - FreeRTOS Software Timer Handle
 * @addtogroup FreeRTOS
 */
static void app_clock_idle_timer_unregist(xTimerHandle xTimer){
  ASSERT(pdPASS==xTimerDelete(xTimer, 0), "Timer is NOT deleted");
}





/* ************************************************************************** */
/*                          Public Clock UI Function                          */
/* ************************************************************************** */
/**
 * @brief Clock Application Main Entrance
 * @param [in] param - will cast to `tAppClock*`
 */
void app_clock_main(void *param) RTOSTHREAD APP_CLOCK_GLOBAL{
  tAppClock *parsed_param = (tAppClock *)param;

  app_clock_gui_switch(parsed_param->style);
  
  xEventGroupSetBits(metope.app.rtos.event._handle, CMN_EVENT_UPDATE_RTC);
  
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

    EventBits_t uxBits = xEventGroupWaitBits( metope.app.rtos.event._handle, CMN_EVENT_UPDATE_RTC, pdFALSE, pdFALSE, DEFAULT_CLOCK_REFREASH_PERIOD);

    if(uxBits & CMN_EVENT_UPDATE_RTC){
      vTaskSuspendAll();
      cmnDateTime_t rtc_time = bsp_rtc_get_time();
      xTaskResumeAll();
      parsed_param->gui.set_time( &parsed_param->gui.param, rtc_time);
      /**
       * @bug 
       *  LVGL can not finish a correct partial refreash after a big needle angle change
       */
      lv_obj_invalidate(parsed_param->gui.param.pScreen);
      xEventGroupClearBits(metope.app.rtos.event._handle, CMN_EVENT_UPDATE_RTC);
    }else{
      /**
       * @note
       *  View Part:
       *    1) Update the increased ms.
       */
      parsed_param->gui.inc_time( &parsed_param->gui.param, ms_delta);
    }
  }
}


/**
 * @brief Clock Application Idle Program
 * @note 
 *  This function should be run when system is in idle. It is optional and 
 *  depends on what style of this clock. Some types of the clock ui may NOT
 *  need an idle program.
 * @note 
 *  Application can run correctly in a short time without idle program presence
 * @param [in] param - will cast to `tAppClock*`
 */
void app_clock_idle(void *param) RTOSIDLE APP_CLOCK_GLOBAL{
  tAppClock *parsed_param = (tAppClock *)param;
  if(NULL!=parsed_param->gui.idle){
    parsed_param->gui.idle(&parsed_param->gui.param);
    xTimerReset(parsed_param->gui.param._idle_task_timer, 0);
  }
}


#ifdef __cplusplus
}
#endif
