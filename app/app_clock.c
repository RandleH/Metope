/**
 ******************************************************************************
 * @file    app_clock.c
 * @author  RandleH
 * @brief   Application Program - Clock
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
#include "app_lvgl.h"
#include "app_clock.h"
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
#ifdef __cplusplus
extern "C"{
#endif

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

#ifdef __cplusplus
}
#endif



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
 * @brief Analog Clock Idle (Clean Up) Fcuntion
 * @param [inout] pClient - The UI Widget Structure Variable
 * @param [inout] params  - The Abstract Analog Clock Parameters
 * @note  Any clean up or less priority task can be put here
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
/*                  Private Clock UI Function - Clock Modern                  */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
  tAnalogClockInternalParam  analog_clk;
  lv_obj_t *ui_sun;
  lv_obj_t *ui_moon;
  lv_obj_t *ui_weekday_obj[kWeekDay_TOTAL];
  lv_obj_t *ui_weekday_mat[kWeekDay_TOTAL];
  lv_color_t ui_weekday_color[kWeekDay_TOTAL+1]; /*!< Last Element will be inactive color */
  lv_obj_t *ui_battery;
}tClockModernInternalParam;

static void ui_clockmodern_init    (tAppGuiClockParam *pClient)                     APP_CLOCK_API;
static void ui_clockmodern_set_time(tAppGuiClockParam *pClient, cmnDateTime_t time) APP_CLOCK_API;
static void ui_clockmodern_inc_time(tAppGuiClockParam *pClient, uint32_t ms)        APP_CLOCK_API;
static void ui_clockmodern_idle    (tAppGuiClockParam *pClient)                     APP_CLOCK_API;
static void ui_clockmodern_deinit  (tAppGuiClockParam *pClient)                     APP_CLOCK_API;
static void ui_clockmodern_set_weekday(tClockModernInternalParam *pClientPrivateParams, cmnWeekday_t weekday);
static void ui_clockmodern_daynight   (tClockModernInternalParam *pClientPrivateParams, cmnDateTime_t time);

/**
 * @brief UI Clock Modern Initialization
 * @param [out] pClient - The UI Widget Structure Variable
 * @note UI Clock Modern has no special parameters. `pClient->p_anything` will set to `NULL`
 * @addtogroup ThreadSafe
 */
static void ui_clockmodern_init(tAppGuiClockParam *pClient) APP_CLOCK_API {
  pClient->customized._semphr = xSemaphoreCreateMutex();
  ASSERT(pClient->customized._semphr, "Mutex was NOT created");
  BaseType_t                 ret                  = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  tClockModernInternalParam *pClientPrivateParams = (tClockModernInternalParam *)pvPortMalloc(sizeof(tClockModernInternalParam));

  ///////////////////////////////////////////////////////////////
  /////////////////////// Safe Zone Start ///////////////////////
  ASSERT(ret==pdTRUE, "Data was NOT obtained");

  // pClient->pScreen = lv_scr_act();
  pClient->pScreen = lv_obj_create(NULL);

  lv_obj_clear_flag(pClient->pScreen, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
  lv_obj_set_style_bg_color(pClient->pScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(pClient->pScreen, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

  /**
   * @note: Clock Exterior Panel
   * @remarks UI
   */
  {
    lv_obj_t *ui_ClockExteriorPanel = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_ClockExteriorPanel, 240);
    lv_obj_set_height( ui_ClockExteriorPanel, 240);
    lv_obj_set_align( ui_ClockExteriorPanel, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_ClockExteriorPanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_ClockExteriorPanel, 240, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ClockExteriorPanel, lv_color_hex(0x525151), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_ClockExteriorPanel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_ClockExteriorPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_ClockExteriorPanel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_ClockExteriorPanel, 20, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_ClockExteriorPanel, LV_BORDER_SIDE_FULL, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_ClockExteriorPanel, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);

    /**
     * @note: Clock Panel Digits
     */
#if 0 // Screen Messed up
    for( u8 i=0; i<6; ++i){
      lv_obj_t *ui_PitDigits = lv_obj_create(ui_ClockExteriorPanel);
      if(i==0 || i==3){
        lv_obj_set_width( ui_PitDigits, 4);
      }else{
        lv_obj_set_width( ui_PitDigits, 2);
      }
      lv_obj_set_height( ui_PitDigits, 240);
      lv_obj_set_align( ui_PitDigits, LV_ALIGN_CENTER );
      lv_obj_clear_flag( ui_PitDigits, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
      lv_obj_set_style_transform_angle( ui_PitDigits, i*300, LV_PART_MAIN| LV_STATE_DEFAULT);
      lv_obj_set_style_transform_pivot_x( ui_PitDigits, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
      lv_obj_set_style_transform_pivot_y( ui_PitDigits, 120, LV_PART_MAIN| LV_STATE_DEFAULT);
    }
#else
    const lv_coord_t dot_position[][2] = {
      { 82,-48},
      { 95,  0},
      { 82, 48},
      { 48, 82},
      {  0, 95},
      {-48, 82},
      {-82, 48},
      {-95,  0},
      {-82,-48}
    };

    for( u8 i=0; i<sizeof(dot_position)/sizeof(*dot_position); ++i){
      lv_obj_t *ui_dot = lv_obj_create(pClient->pScreen);
      if(i==(3-2)||i==(6-2)||i==(9-2)){
        if(i==(6-2)){
          lv_obj_set_width( ui_dot, 6);
          lv_obj_set_height( ui_dot, 12);
        }else{
          lv_obj_set_width( ui_dot, 12);
          lv_obj_set_height( ui_dot, 6);
        }
      }else{
        lv_obj_set_width( ui_dot, 6);
        lv_obj_set_height( ui_dot, 6);
      }
      lv_obj_set_x( ui_dot, dot_position[i][0]);
      lv_obj_set_y( ui_dot, dot_position[i][1]);
      lv_obj_set_align( ui_dot, LV_ALIGN_CENTER );
      lv_obj_clear_flag( ui_dot, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
      if(i==(3-2)||i==(6-2)||i==(9-2)){
        lv_obj_set_style_radius(ui_dot, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui_dot, lv_color_hex(0xFF8200), LV_PART_MAIN | LV_STATE_DEFAULT );
      }else{
        lv_obj_set_style_radius(ui_dot, 10, LV_PART_MAIN| LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui_dot, lv_color_hex(0xFFE000), LV_PART_MAIN | LV_STATE_DEFAULT );
      }
      lv_obj_set_style_bg_opa(ui_dot, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
      lv_obj_set_style_border_width(ui_dot, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    }
#endif
  }

  /**
   * @note: Clock Interior Panel
   * @remarks UI
   */
#if 1
  {
    lv_obj_t *ui_ClockInteriorPanel = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_ClockInteriorPanel, 180);
    lv_obj_set_height( ui_ClockInteriorPanel, 180);
    lv_obj_set_align( ui_ClockInteriorPanel, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_ClockInteriorPanel, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_ClockInteriorPanel, 240, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_ClockInteriorPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_ClockInteriorPanel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_ClockInteriorPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_ClockInteriorPanel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_ClockInteriorPanel, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  }
#endif

#if 1
  /**
   * @note: Sun & Moon
   */
  {
    lv_obj_t *ui_Sun = lv_img_create(pClient->pScreen);
    lv_img_set_src(ui_Sun, &ui_img_sun_32);
    lv_obj_set_width( ui_Sun, LV_SIZE_CONTENT);  /// 32
    lv_obj_set_height( ui_Sun, LV_SIZE_CONTENT);   /// 32
    lv_obj_set_x( ui_Sun, 0 );
    lv_obj_set_y( ui_Sun, 40 );
    lv_obj_set_align( ui_Sun, LV_ALIGN_CENTER );
    lv_obj_add_flag( ui_Sun, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
    lv_obj_clear_flag( ui_Sun, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    pClientPrivateParams->ui_sun = ui_Sun;
    
    lv_obj_t *ui_Moon = lv_img_create(pClient->pScreen);
    lv_img_set_src(ui_Moon, &ui_img_moon_32);
    lv_obj_set_width( ui_Moon, LV_SIZE_CONTENT);  /// 32
    lv_obj_set_height( ui_Moon, LV_SIZE_CONTENT);   /// 32
    lv_obj_set_x( ui_Moon, 0 );
    lv_obj_set_y( ui_Moon, 40 );
    lv_obj_set_align( ui_Moon, LV_ALIGN_CENTER );
    lv_obj_add_flag( ui_Moon, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
    lv_obj_clear_flag( ui_Moon, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    pClientPrivateParams->ui_moon = ui_Moon;
  }
#endif

  /**
   * @note
   *  `Sunday`|`Monday`|`Tuesday`|`Wednesday`|`Thursday`|`Friday`|`Saturday`
   */
#if 1
  {
    const lv_coord_t weekday_icon_position[kWeekDay_TOTAL][2] = {
      {-61,-35},
      {-35,-61},
      {  0,-70},
      { 35,-61},
      { 61,-35},
      { 70,  0},
      {-70,  0}
    };

    const u16 weekday_icon_angle[kWeekDay_TOTAL] = {
      3000, 3300, 0, 300, 600, 900, 2700
    };

    const lv_img_dsc_t *weekday_icon_img[kWeekDay_TOTAL] = {
      &ui_img_ball_M_24,
      &ui_img_ball_T_24,
      &ui_img_ball_W_24,
      &ui_img_ball_T_24,
      &ui_img_ball_F_24,
      &ui_img_ball_S_24,
      &ui_img_ball_S_24
    };

    const u8 weekday_mat_icon_size = 22;

    pClientPrivateParams->ui_weekday_color[kWeekDay_Monday]    = lv_color_hex(0xFFAB00);
    pClientPrivateParams->ui_weekday_color[kWeekDay_Tuesday]   = lv_color_hex(0xD4D400);
    pClientPrivateParams->ui_weekday_color[kWeekDay_Wednesday] = lv_color_hex(0x00FF05);
    pClientPrivateParams->ui_weekday_color[kWeekDay_Thursday]  = lv_color_hex(0x0082FB);
    pClientPrivateParams->ui_weekday_color[kWeekDay_Friday]    = lv_color_hex(0x2028FF);
    pClientPrivateParams->ui_weekday_color[kWeekDay_Saturday]  = lv_color_hex(0xD100FB);
    pClientPrivateParams->ui_weekday_color[kWeekDay_Sunday]    = lv_color_hex(0xFF2200);
    pClientPrivateParams->ui_weekday_color[kWeekDay_TOTAL]     = lv_color_hex(0x3E3E3E);

    for(u8 weekday=kWeekDay_Monday; weekday<kWeekDay_TOTAL; ++weekday){
      pClientPrivateParams->ui_weekday_mat[weekday] = lv_obj_create(pClient->pScreen);
      lv_obj_set_width( pClientPrivateParams->ui_weekday_mat[weekday],weekday_mat_icon_size);
      lv_obj_set_height( pClientPrivateParams->ui_weekday_mat[weekday], weekday_mat_icon_size);
      lv_obj_set_x( pClientPrivateParams->ui_weekday_mat[weekday], weekday_icon_position[weekday][0]);
      lv_obj_set_y( pClientPrivateParams->ui_weekday_mat[weekday], weekday_icon_position[weekday][1]);
      lv_obj_set_align( pClientPrivateParams->ui_weekday_mat[weekday], LV_ALIGN_CENTER );
      lv_obj_add_flag( pClientPrivateParams->ui_weekday_mat[weekday], LV_OBJ_FLAG_HIDDEN );   /// Flags
      lv_obj_clear_flag( pClientPrivateParams->ui_weekday_mat[weekday], LV_OBJ_FLAG_SCROLLABLE );    /// Flags
      lv_obj_set_style_radius( pClientPrivateParams->ui_weekday_mat[weekday], weekday_mat_icon_size, LV_PART_MAIN| LV_STATE_DEFAULT);
      lv_obj_set_style_bg_color( pClientPrivateParams->ui_weekday_mat[weekday], lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_bg_opa( pClientPrivateParams->ui_weekday_mat[weekday], 255, LV_PART_MAIN| LV_STATE_DEFAULT);
      lv_obj_set_style_shadow_color( pClientPrivateParams->ui_weekday_mat[weekday], pClientPrivateParams->ui_weekday_color[weekday], LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_shadow_opa( pClientPrivateParams->ui_weekday_mat[weekday], 255, LV_PART_MAIN| LV_STATE_DEFAULT);
      lv_obj_set_style_shadow_width( pClientPrivateParams->ui_weekday_mat[weekday], 10, LV_PART_MAIN| LV_STATE_DEFAULT);
      lv_obj_set_style_shadow_spread( pClientPrivateParams->ui_weekday_mat[weekday], 2, LV_PART_MAIN| LV_STATE_DEFAULT);
      
      pClientPrivateParams->ui_weekday_obj[weekday] = lv_img_create(pClient->pScreen);
      lv_img_set_src( pClientPrivateParams->ui_weekday_obj[weekday], weekday_icon_img[weekday]);
      lv_obj_set_width( pClientPrivateParams->ui_weekday_obj[weekday], LV_SIZE_CONTENT);  /// 24
      lv_obj_set_height( pClientPrivateParams->ui_weekday_obj[weekday], LV_SIZE_CONTENT);   /// 24
      lv_obj_set_x( pClientPrivateParams->ui_weekday_obj[weekday], weekday_icon_position[weekday][0]);
      lv_obj_set_y( pClientPrivateParams->ui_weekday_obj[weekday], weekday_icon_position[weekday][1]);
      lv_obj_set_align( pClientPrivateParams->ui_weekday_obj[weekday], LV_ALIGN_CENTER );
      lv_obj_add_flag( pClientPrivateParams->ui_weekday_obj[weekday], LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
      lv_obj_clear_flag( pClientPrivateParams->ui_weekday_obj[weekday], LV_OBJ_FLAG_SCROLLABLE );    /// Flags
      lv_img_set_angle( pClientPrivateParams->ui_weekday_obj[weekday], weekday_icon_angle[weekday]);

      lv_obj_set_style_img_recolor( pClientPrivateParams->ui_weekday_obj[weekday], pClientPrivateParams->ui_weekday_color[kWeekDay_TOTAL], LV_PART_MAIN| LV_STATE_DEFAULT);
      
      lv_obj_set_style_img_recolor_opa( pClientPrivateParams->ui_weekday_obj[weekday], 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    }
  }
#endif
  /**
   * @note: Hour Hand Needle
   */
  {
#if 0 // Screen Messed up
    lv_obj_t *ui_PinHour = lv_img_create(pClient->pScreen);
    lv_img_set_src(ui_PinHour, &ui_img_pin_hour_type1);
    lv_obj_set_width( ui_PinHour, LV_SIZE_CONTENT);  /// 10
    lv_obj_set_height( ui_PinHour, LV_SIZE_CONTENT);   /// 64
    lv_obj_set_x( ui_PinHour, 0 );
    lv_obj_set_y( ui_PinHour, -115 );
    lv_obj_set_align( ui_PinHour, LV_ALIGN_BOTTOM_MID );
    lv_obj_add_flag( ui_PinHour, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
    lv_obj_clear_flag( ui_PinHour, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_img_set_pivot(ui_PinHour,5,59);
    lv_img_set_angle(ui_PinHour,90);
    lv_obj_set_style_img_recolor(ui_PinHour, lv_color_hex(0xE65D31), LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_PinHour, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    pClient->pPinHour = ui_PinHour;
#else
    const u8 ui_NeedleLen   = 50;
    const u8 ui_NeedleWidth = 8;
    lv_obj_t *ui_PinHour = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_PinHour, ui_NeedleWidth);
    lv_obj_set_height( ui_PinHour, ui_NeedleLen);
    lv_obj_set_x( ui_PinHour, 0 );
    lv_obj_set_y( ui_PinHour, -(signed)(BSP_SCREEN_HEIGHT/2-ui_NeedleWidth/2));
    lv_obj_set_align( ui_PinHour, LV_ALIGN_BOTTOM_MID );
    lv_obj_clear_flag( ui_PinHour, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_bg_color(ui_PinHour, lv_color_hex(0xE65D31), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_PinHour, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_PinHour, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_PinHour, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_angle(ui_PinHour, 2700, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_x(ui_PinHour, ui_NeedleWidth/2, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_y(ui_PinHour, ui_NeedleLen-ui_NeedleWidth/2, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_PinHour, lv_color_hex(0x000000), LV_PART_SCROLLBAR | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_PinHour, 0, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);
    pClient->pPinHour = ui_PinHour;
#endif
  }
  
  /**
   * @note: Minute Hand Needle
   */
  {
#if 0 // Screen Messed up
    lv_obj_t *ui_PinMinute = lv_img_create(pClient->pScreen);
    lv_img_set_src(ui_PinMinute, &ui_img_pin_minute_type3);
    lv_obj_set_width( ui_PinMinute, LV_SIZE_CONTENT);  /// 10
    lv_obj_set_height( ui_PinMinute, LV_SIZE_CONTENT);   /// 71
    lv_obj_set_x( ui_PinMinute, 0 );
    lv_obj_set_y( ui_PinMinute, -115 );
    lv_obj_set_align( ui_PinMinute, LV_ALIGN_BOTTOM_MID );
    lv_obj_add_flag( ui_PinMinute, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
    lv_obj_clear_flag( ui_PinMinute, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_img_set_pivot(ui_PinMinute,5,66);
    lv_img_set_angle(ui_PinMinute,900);
    lv_obj_set_style_img_recolor(ui_PinMinute, lv_color_hex(0xCA8D7D), LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_PinMinute, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    pClient->pPinMinute = ui_PinMinute;
#else
    const u8 ui_NeedleLen   = 71;
    const u8 ui_NeedleWidth = 8;
    lv_obj_t *ui_PinMin = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_PinMin, ui_NeedleWidth);
    lv_obj_set_height( ui_PinMin, ui_NeedleLen);
    lv_obj_set_x( ui_PinMin, 0 );
    lv_obj_set_y( ui_PinMin, -(signed)(BSP_SCREEN_HEIGHT/2-ui_NeedleWidth/2));
    lv_obj_set_align( ui_PinMin, LV_ALIGN_BOTTOM_MID );
    lv_obj_clear_flag( ui_PinMin, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_bg_color(ui_PinMin, lv_color_hex(0xCA8D7D), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_PinMin, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_PinMin, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_color(ui_PinMin, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_PinMin, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_angle(ui_PinMin, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_x(ui_PinMin, ui_NeedleWidth/2, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_transform_pivot_y(ui_PinMin, ui_NeedleLen - ui_NeedleWidth/2, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_PinMin, lv_color_hex(0xFFFFFF), LV_PART_SCROLLBAR | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_PinMin, 255, LV_PART_SCROLLBAR| LV_STATE_DEFAULT);
    pClient->pPinMinute = ui_PinMin;
#endif
  }

  /**
   * @note: Knotch
   */
#if 1
  {
    lv_obj_t *ui_Knotch = lv_obj_create(pClient->pScreen);
    lv_obj_set_width( ui_Knotch, 20);
    lv_obj_set_height( ui_Knotch, 20);
    lv_obj_set_align( ui_Knotch, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_Knotch, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_Knotch, 20, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_Knotch, lv_color_hex(0xC1670A), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_main_stop(ui_Knotch, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui_Knotch, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_Knotch, LV_GRAD_DIR_VER, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Knotch, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_opa(ui_Knotch, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  }
#endif

  /**
   * @note: Battery
   */
#if 1
  {
    lv_obj_t *ui_Battery = lv_arc_create(pClient->pScreen);
    lv_obj_set_width( ui_Battery, 225);
    lv_obj_set_height( ui_Battery, 225);
    lv_obj_set_align( ui_Battery, LV_ALIGN_CENTER );
    lv_arc_set_range(ui_Battery, 0,255);
    lv_arc_set_value(ui_Battery, 190);
    lv_arc_set_bg_angles(ui_Battery,230,310);
    lv_obj_set_style_arc_color(ui_Battery, lv_color_hex(0x3E3E3E), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_arc_opa(ui_Battery, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_Battery, 6, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_Battery, lv_color_hex(0x00DF11), LV_PART_INDICATOR | LV_STATE_DEFAULT );
    lv_obj_set_style_arc_opa(ui_Battery, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_Battery, 6, LV_PART_INDICATOR| LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_Battery, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_Battery, 0, LV_PART_KNOB| LV_STATE_DEFAULT);

    pClientPrivateParams->ui_battery = ui_Battery;
  }
#endif

  /**
   * @note: Text for digits
   */
#if 1
  {
    lv_obj_t *ui_Txt2 = lv_label_create(pClient->pScreen);
    lv_obj_set_width( ui_Txt2, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height( ui_Txt2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x( ui_Txt2, 95 );
    lv_obj_set_y( ui_Txt2, -55 );
    lv_obj_set_align( ui_Txt2, LV_ALIGN_CENTER );
    lv_label_set_text(ui_Txt2,"2");
    lv_obj_set_style_text_color(ui_Txt2, lv_color_hex(0x9E9E9E), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_text_opa(ui_Txt2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_Txt3 = lv_label_create(pClient->pScreen);
    lv_obj_set_width( ui_Txt3, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height( ui_Txt3, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x( ui_Txt3, 110 );
    lv_obj_set_y( ui_Txt3, 0 );
    lv_obj_set_align( ui_Txt3, LV_ALIGN_CENTER );
    lv_label_set_text(ui_Txt3,"3");
    lv_obj_set_style_text_color(ui_Txt3, lv_color_hex(0x9E9E9E), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_text_opa(ui_Txt3, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_Txt4 = lv_label_create(pClient->pScreen);
    lv_obj_set_width( ui_Txt4, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height( ui_Txt4, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x( ui_Txt4, 95 );
    lv_obj_set_y( ui_Txt4, 55 );
    lv_obj_set_align( ui_Txt4, LV_ALIGN_CENTER );
    lv_label_set_text(ui_Txt4,"4");
    lv_obj_set_style_text_color(ui_Txt4, lv_color_hex(0x9E9E9E), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_text_opa(ui_Txt4, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_Txt5 = lv_label_create(pClient->pScreen);
    lv_obj_set_width( ui_Txt5, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height( ui_Txt5, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x( ui_Txt5, 55 );
    lv_obj_set_y( ui_Txt5, 95 );
    lv_obj_set_align( ui_Txt5, LV_ALIGN_CENTER );
    lv_label_set_text(ui_Txt5,"5");
    lv_obj_set_style_text_color(ui_Txt5, lv_color_hex(0x9E9E9E), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_text_opa(ui_Txt5, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_Txt6 = lv_label_create(pClient->pScreen);
    lv_obj_set_width( ui_Txt6, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height( ui_Txt6, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x( ui_Txt6, 0 );
    lv_obj_set_y( ui_Txt6, 110 );
    lv_obj_set_align( ui_Txt6, LV_ALIGN_CENTER );
    lv_label_set_text(ui_Txt6,"6");
    lv_obj_set_style_text_color(ui_Txt6, lv_color_hex(0x9E9E9E), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_text_opa(ui_Txt6, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_Txt7 = lv_label_create(pClient->pScreen);
    lv_obj_set_width( ui_Txt7, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height( ui_Txt7, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x( ui_Txt7, -55 );
    lv_obj_set_y( ui_Txt7, 95 );
    lv_obj_set_align( ui_Txt7, LV_ALIGN_CENTER );
    lv_label_set_text(ui_Txt7,"7");
    lv_obj_set_style_text_color(ui_Txt7, lv_color_hex(0x9E9E9E), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_text_opa(ui_Txt7, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_Txt8 = lv_label_create(pClient->pScreen);
    lv_obj_set_width( ui_Txt8, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height( ui_Txt8, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x( ui_Txt8, -95 );
    lv_obj_set_y( ui_Txt8, 55 );
    lv_obj_set_align( ui_Txt8, LV_ALIGN_CENTER );
    lv_label_set_text(ui_Txt8,"8");
    lv_obj_set_style_text_color(ui_Txt8, lv_color_hex(0x9E9E9E), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_text_opa(ui_Txt8, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_Txt9 = lv_label_create(pClient->pScreen);
    lv_obj_set_width( ui_Txt9, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height( ui_Txt9, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x( ui_Txt9, -110 );
    lv_obj_set_y( ui_Txt9, 0 );
    lv_obj_set_align( ui_Txt9, LV_ALIGN_CENTER );
    lv_label_set_text(ui_Txt9,"9");
    lv_obj_set_style_text_color(ui_Txt9, lv_color_hex(0x9E9E9E), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_text_opa(ui_Txt9, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

    lv_obj_t *ui_Txt10 = lv_label_create(pClient->pScreen);
    lv_obj_set_width( ui_Txt10, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height( ui_Txt10, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x( ui_Txt10, -95 );
    lv_obj_set_y( ui_Txt10, -55 );
    lv_obj_set_align( ui_Txt10, LV_ALIGN_CENTER );
    lv_label_set_text(ui_Txt10,"10");
    lv_obj_set_style_text_color(ui_Txt10, lv_color_hex(0x9E9E9E), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_text_opa(ui_Txt10, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  }
#endif

  pClient->customized.p_anything = pClientPrivateParams;

  pClient->_idle_task_timer = app_clock_idle_timer_regist();
  lv_scr_load(pClient->pScreen);

  xTimerStart(pClient->_idle_task_timer, 0);
  //////////////////////// Safe Zone End ////////////////////////
  ///////////////////////////////////////////////////////////////
  ret = xSemaphoreGive(pClient->customized._semphr);
  ASSERT(ret==pdTRUE, "Data was NOT released");
}

/**
 * @brief UI Clock Modern Set Time
 * @param [inout] pClient - The UI Widget Structure Variable
 * @param [in]    time    - Date Time
 * @note Update needle angle
 * @addtogroup ThreadSafe
 */
static void ui_clockmodern_set_time(tAppGuiClockParam *pClient, cmnDateTime_t time) APP_CLOCK_API {
  tClockModernInternalParam *pClientPrivateParams = (tClockModernInternalParam *)pClient->customized.p_anything;
  
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ///////////////////////////////////////////////////////////////
  /////////////////////// Safe Zone Start ///////////////////////
  ASSERT(ret==pdTRUE, "Data was NOT obtained");
  analogclk_set_time( pClient, &pClientPrivateParams->analog_clk, time);
  ui_clockmodern_daynight( pClientPrivateParams, time);
  ui_clockmodern_set_weekday( pClientPrivateParams, cmn_utility_get_weekday(time));
  //////////////////////// Safe Zone End ////////////////////////
  ///////////////////////////////////////////////////////////////
  xSemaphoreGive(pClient->customized._semphr);
}

/**
 * @brief UI Clock Modern Set the weekday icon on the clock panel
 * @param [inout] pClientPrivateParams - Clock Private Parameters
 * @param [in]    weekday              - Weekday
 * @addtogroup ThreadNotSafe
 */
static void ui_clockmodern_set_weekday(tClockModernInternalParam *pClientPrivateParams, cmnWeekday_t weekday){
  for(u8 day=0; day<kWeekDay_TOTAL; ++day){
    if(day==weekday){
      lv_obj_clear_flag(pClientPrivateParams->ui_weekday_mat[weekday], LV_OBJ_FLAG_HIDDEN);
      lv_obj_set_style_img_recolor( pClientPrivateParams->ui_weekday_obj[weekday], pClientPrivateParams->ui_weekday_color[weekday], LV_PART_MAIN| LV_STATE_DEFAULT);
    }else{
      if(!lv_obj_has_flag(pClientPrivateParams->ui_weekday_mat[day], LV_OBJ_FLAG_HIDDEN)){
        lv_obj_add_flag( pClientPrivateParams->ui_weekday_mat[day], LV_OBJ_FLAG_HIDDEN );
        lv_obj_set_style_img_recolor( pClientPrivateParams->ui_weekday_obj[day], pClientPrivateParams->ui_weekday_color[kWeekDay_TOTAL], LV_PART_MAIN| LV_STATE_DEFAULT);
      }
    }
  }
}

/**
 * @brief UI Clock Modern Set Day Night
 * @param [inout] pClientPrivateParams - Clock Private Parameters
 * @param [in]    time                 - Time
 * @note Update sun / moon icon
 * @addtogroup NotThreadSafe
 */
static void ui_clockmodern_daynight(tClockModernInternalParam *pClientPrivateParams, cmnDateTime_t time){
  if(time.hour >= 18){
    if(!lv_obj_has_flag( pClientPrivateParams->ui_sun, LV_OBJ_FLAG_HIDDEN) || lv_obj_has_flag(pClientPrivateParams->ui_moon, LV_OBJ_FLAG_HIDDEN)){ 
      /* Moon Rise */
      lv_obj_add_flag( pClientPrivateParams->ui_sun, LV_OBJ_FLAG_HIDDEN );
      lv_obj_clear_flag( pClientPrivateParams->ui_moon, LV_OBJ_FLAG_HIDDEN);
    }
  }else if(time.hour >= 8){
    if(lv_obj_has_flag(pClientPrivateParams->ui_sun, LV_OBJ_FLAG_HIDDEN) || !lv_obj_has_flag(pClientPrivateParams->ui_moon, LV_OBJ_FLAG_HIDDEN)){
      /* Sun Rise */
      lv_obj_add_flag( pClientPrivateParams->ui_moon, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag( pClientPrivateParams->ui_sun, LV_OBJ_FLAG_HIDDEN);
    }
  }
}

/**
 * @brief UI Clock Modern Inc Time (ms)
 * @param [inout] pClient - The UI Widget Structure Variable
 * @param [in]    ms      - Escaped Microseconds
 * @note Update needle angle
 * @addtogroup ThreadSafe
 */
static void ui_clockmodern_inc_time(tAppGuiClockParam *pClient, uint32_t ms) APP_CLOCK_API {
  tClockModernInternalParam *pClientPrivateParams = (tClockModernInternalParam *)pClient->customized.p_anything;
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ///////////////////////////////////////////////////////////////
  /////////////////////// Safe Zone Start ///////////////////////
  ASSERT(ret==pdTRUE, "Data was NOT obtained");
  analogclk_inc_time( pClient, &pClientPrivateParams->analog_clk, ms);
  //////////////////////// Safe Zone End ////////////////////////
  ///////////////////////////////////////////////////////////////
  xSemaphoreGive(pClient->customized._semphr);
}

/**
 * @brief UI Clock Modern Idle Execution
 * @note Mathmatical Modulo / Time Adjustment
 * @param [inout] pClient - The UI Widget Structure Variable
 * @addtogroup ThreadSafe
 */
static void ui_clockmodern_idle(tAppGuiClockParam *pClient) APP_CLOCK_API {
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ///////////////////////////////////////////////////////////////
  /////////////////////// Safe Zone Start ///////////////////////
  ASSERT(ret==pdTRUE, "Data was NOT obtained");
  tClockModernInternalParam *pClientPrivateParams = (tClockModernInternalParam *)pClient->customized.p_anything;
  analogclk_idle(pClient, &pClientPrivateParams->analog_clk);
  
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
 * @brief UI Clock Modern Deinitialization
 * @param [inout] pClient - The UI Widget Structure Variable
 * @addtogroup ThreadSafe
 */
static void ui_clockmodern_deinit(tAppGuiClockParam *pClient) APP_CLOCK_API {
  SemaphoreHandle_t _semphr = pClient->customized._semphr;
  BaseType_t            ret = xSemaphoreTake( _semphr, portMAX_DELAY);
  ASSERT(ret==pdTRUE, "Data was NOT obtained");

  ///////////////////////////////////////////////////////////////
  /////////////////////// Safe Zone Start ///////////////////////
  app_lvgl_load_default_screen();
  lv_obj_del(pClient->pScreen);
  {
    tClockModernInternalParam *pClientPrivateParams = (tClockModernInternalParam *)pClient->customized.p_anything;
    memset(pClientPrivateParams, 0, sizeof(tClockModernInternalParam));
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
static void ui_clocknana_idle    (tAppGuiClockParam *pClient)                     APP_CLOCK_API;
static void ui_clocknana_deinit  (tAppGuiClockParam *pClient)                     APP_CLOCK_API;
static void ui_clocknana_daynight(tClockNanaInternalParam *pClientPrivateParams, cmnDateTime_t time);

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
 * @param [inout] pClientPrivateParams - Clock Private Parameters
 * @param [in]    time                 - Time
 * @note Update NaNa's eyes
 * @addtogroup NotThreadSafe
 */
static void ui_clocknana_daynight(tClockNanaInternalParam *pClientPrivateParams, cmnDateTime_t time){
  lv_obj_t *ui_nanaeyeopen   = pClientPrivateParams->ui_nanaeyeopen;
  lv_obj_t *ui_nanaeyeclosed = pClientPrivateParams->ui_nanaeyeclosed;

  if(time.hour >= 18){
    if(!lv_obj_has_flag(ui_nanaeyeopen, LV_OBJ_FLAG_HIDDEN) || lv_obj_has_flag(ui_nanaeyeclosed, LV_OBJ_FLAG_HIDDEN)){ 
      /* Close Eyes */
      lv_obj_add_flag( ui_nanaeyeopen, LV_OBJ_FLAG_HIDDEN );
      lv_obj_clear_flag( ui_nanaeyeclosed, LV_OBJ_FLAG_HIDDEN);
    }
  }else if(time.hour >= 8){
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
 * @param [in]    time    - Date Time
 * @addtogroup ThreadSafe
 */
static void ui_clocknana_set_time(tAppGuiClockParam *pClient, cmnDateTime_t time){
  tClockNanaInternalParam *pClientPrivateParams = (tClockNanaInternalParam *)pClient->customized.p_anything;
  
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ///////////////////////////////////////////////////////////////
  /////////////////////// Safe Zone Start ///////////////////////
  ASSERT(ret==pdTRUE, "Data was NOT obtained");

  analogclk_set_time( pClient, &pClientPrivateParams->analog_clk, time);
  ui_clocknana_daynight(pClientPrivateParams, pClient->time);
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
  ui_clocknana_daynight(pClientPrivateParams, pClient->time);
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

static void ui_clocklvvvw_init    (tAppGuiClockParam *pClient)                     APP_CLOCK_API;
static void ui_clocklvvvw_set_time(tAppGuiClockParam *pClient, cmnDateTime_t time) APP_CLOCK_API;
static void ui_clocklvvvw_inc_time(tAppGuiClockParam *pClient, uint32_t ms)        APP_CLOCK_API;
static void ui_clocklvvvw_idle    (tAppGuiClockParam *pClient)                     APP_CLOCK_API;
static void ui_clocklvvvw_deinit  (tAppGuiClockParam *pClient)                     APP_CLOCK_API;

typedef struct stClockLvvvwInternalParam{
  lv_color_t                 ruby_color;
  lv_obj_t                  *ui_ruby;
  tAnalogClockInternalParam  analog_clk;
} tClockLvvvwInternalParam;

/**
 * @brief UI Clock Lvvvw Initialization
 * @param [inout] pClient - The UI Widget Structure Variable
 * @addtogroup ThreadSafe
 */
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

/**
 * @brief UI Clock Lvvvw Set Time
 * @param [inout] pClient - The UI Widget Structure Variable
 * @param [in]    time    - Date Time
 * @addtogroup ThreadSafe
 */
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

/**
 * @brief UI Clock Lvvvw Increase Time
 * @param [inout] pClient - The UI Widget Structure Variable
 * @param [in]    ms      - Escaped Microseconds
 * @addtogroup ThreadSafe
 */
static void ui_clocklvvvw_inc_time(tAppGuiClockParam *pClient, uint32_t ms){
  tClockLvvvwInternalParam *pClientPrivateParams = (tClockLvvvwInternalParam *)pClient->customized.p_anything;
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ASSERT(ret==pdTRUE, "Data was NOT obtained");

  analogclk_inc_time( pClient, &pClientPrivateParams->analog_clk, ms);
  
  xSemaphoreGive(pClient->customized._semphr);
}

/**
 * @brief UI Clock Lvvvw Idle Execution
 * @note Mathmatical Modulo / Time Adjustment
 * @param [inout] pClient - The UI Widget Structure Variable
 * @addtogroup ThreadSafe
 */
static void ui_clocklvvvw_idle(tAppGuiClockParam *pClient){
  tClockLvvvwInternalParam *pClientPrivateParams = (tClockLvvvwInternalParam *)pClient->customized.p_anything;
  BaseType_t ret = xSemaphoreTake(pClient->customized._semphr, portMAX_DELAY);
  ASSERT(ret==pdTRUE, "Data was NOT obtained");

  analogclk_idle( pClient, &pClientPrivateParams->analog_clk);
  
  xSemaphoreGive(pClient->customized._semphr);
}

/**
 * @brief UI Clock Lvvvw Deinitialization
 * @param [inout] pClient - The UI Widget Structure Variable
 * @addtogroup ThreadSafe
 */
static void ui_clocklvvvw_deinit(tAppGuiClockParam *pClient){
  ui_clockmodern_deinit(pClient);
}

#ifdef __cplusplus
}
#endif
#endif  // #ifndef TEST_ONLY



/* ************************************************************************** */
/*                         Private Clock UI Function                          */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Switch the Clock GUI Style
 * @param [in] x  - The GUI Enumeration. `0` means deactivating the clock UI.
 * @warning
 *  This function assumes the `deinit()` is NULL when clock ui is deactivated, vice versa.
 * @addtogroup ThreadSafe
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
    case kAppGuiClock_ClockModern:{
      metope.app.clock.gui.init     = ui_clockmodern_init;
      metope.app.clock.gui.set_time = ui_clockmodern_set_time;
      metope.app.clock.gui.inc_time = ui_clockmodern_inc_time;
      metope.app.clock.gui.idle     = ui_clockmodern_idle;
      metope.app.clock.gui.deinit   = ui_clockmodern_deinit;
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

#ifdef __cplusplus
}
#endif



/* ************************************************************************** */
/*                          Public Clock UI Function                          */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif
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


/* ********************************** EOF *********************************** */
