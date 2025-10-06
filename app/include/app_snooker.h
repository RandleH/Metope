/**
 ******************************************************************************
 * @file    app_snooker.h
 * @author  RandleH
 * @brief   Application Program - Snooker Meters
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
#ifndef APP_SNOOKER_H
#define APP_SNOOKER_H

/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include "lvgl.h"

/* ************************************************************************** */
/*                                   Enums                                    */
/* ************************************************************************** */
typedef enum etAppSnookerPlayer {
  kSnookerPlayer1,
  kSnookerPlayer2,
  kNumSnookerPlayer
} AppSnookerPlayer_t;


typedef enum etAppSnookerBall {
  kSnookerBall_R  ,
  kSnookerBall_Y  ,
  kSnookerBall_G  ,
  kSnookerBall_BRN,
  kSnookerBall_BLU,
  kSnookerBall_P  ,
  kSnookerBall_BLK,
  kSnookerBall_W  ,
  kNumSnookerBall
} AppSnookerBall_t;

/* ************************************************************************** */
/*                                  Objects                                   */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct stAppSnookerStaticBallInfo {
  const lv_color_t color;
  const char       *point;
  const lv_coord_t pos[2];
} tAppSnookerStaticBallInfo;

typedef struct stAppSnookerStaticPlayerInfo {
  const lv_coord_t pos[2];
} tAppSnookerStaticPlayerInfo;

typedef struct stAppSnookerStaticCmn {
  const lv_coord_t ballsize;
  const lv_color_t playercolor_active;
  const lv_color_t playercolor_inactive;
  const lv_coord_t player_icon_rounded_radius;
  const lv_coord_t player_head_size;
  const lv_coord_t player_head_pos;
  const lv_coord_t player_body_size;
  const lv_coord_t player_body_pos[2];
  const lv_coord_t player_body_arc[2];
  const lv_color_t shadow_color;
  const lv_coord_t shadow_width;
  const lv_coord_t shadow_spread;
  const lv_coord_t point_remain_txt_pos[2];
} tAppSnookerStaticCmn;

typedef struct stAppSnookerStatic {
  const tAppSnookerStaticBallInfo   balls[kNumSnookerBall];
  const tAppSnookerStaticPlayerInfo players[kNumSnookerPlayer];
  const tAppSnookerStaticCmn        common;
} tAppSnookerStatic;

typedef struct stAppSnookerDynamicPoints {
  char    str[3+1]; // 3 digits plus '\0' charactor
  uint8_t val;
} tAppSnookerDynamicPoints;

typedef struct stAppSnookerDynamic {
  lv_obj_t *p_screen;
  tAppSnookerDynamicPoints points[kNumSnookerPlayer];
  tAppSnookerDynamicPoints points_rem;
  uint8_t  selected_ball;
  uint8_t  selected_player;
  uint8_t  last_operation;
} tAppSnookerDynamic;

typedef struct stAppSnooker {
  tAppSnookerDynamic       *_dynamic;
  const tAppSnookerStatic  *_static;
} tAppSnooker;


extern       tAppSnookerDynamic g_app_snooker_dynamic;
extern const tAppSnookerStatic  g_app_snooker_static;


void app_snooker_main(void *param) RTOSTHREAD;

#ifdef __cplusplus
}
#endif



#endif
