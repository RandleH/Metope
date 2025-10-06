/**
 ******************************************************************************
 * @file    app_snooker.c
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

/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include "global.h"
#include "assert.h"
#include "trace.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "app_lvgl.h"




/* ************************************************************************** */
/*                                   Globals                                  */
/* ************************************************************************** */
#if 0
  #define THIS (&metope.app.snooker)
#else
  tAppSnooker * const THIS = &metope.app.snooker;
#endif


tAppSnookerDynamic g_app_snooker_dynamic = { 0 };


const tAppSnookerStatic g_app_snooker_static = {
  .balls = {
    [kSnookerBall_R  ] = { .color = {.full=LV_COLOR_HEX(0xBF0202)}, .point =  "1", .pos = {  0,-95} },
    [kSnookerBall_Y  ] = { .color = {.full=LV_COLOR_HEX(0xFFC300)}, .point =  "2", .pos = { 48,-82} },
    [kSnookerBall_G  ] = { .color = {.full=LV_COLOR_HEX(0x297700)}, .point =  "3", .pos = { 82,-48} },
    [kSnookerBall_BRN] = { .color = {.full=LV_COLOR_HEX(0x7F3B03)}, .point =  "4", .pos = { 95,  0} },
    [kSnookerBall_BLU] = { .color = {.full=LV_COLOR_HEX(0x0800F8)}, .point =  "5", .pos = {-95,  0} },
    [kSnookerBall_P  ] = { .color = {.full=LV_COLOR_HEX(0xF800E3)}, .point =  "6", .pos = {-48,-82} },
    [kSnookerBall_BLK] = { .color = {.full=LV_COLOR_HEX(0x464646)}, .point =  "7", .pos = {-82,-48} },
    [kSnookerBall_W  ] = { .color = {.full=LV_COLOR_HEX(0xFFFFFF)}, .point = "-4", .pos = {  0, 95} }
  },
  .players = {
    [kSnookerPlayer1] = { .pos = {-30, 30}},
    [kSnookerPlayer2] = { .pos = { 30, 30}}
  },
  .common = {
    .ballsize             = 32,
    .playercolor_active   = {.full=LV_COLOR_HEX(0xFFFFFF)},
    .playercolor_inactive = {.full=LV_COLOR_HEX(0x646464)},
    .player_icon_rounded_radius  = 12,
    .player_head_size     = 24,
    .player_head_pos      = {0, -10},
    .player_body_size     = 50,
    .player_body_pos      = {0, 30},
    .player_body_arc      = {220,330},
    .shadow_color         = {.full=LV_COLOR_HEX(0x60FFE8)},
    .shadow_width         = 8,
    .shadow_spread        = 4,
    .point_remain_txt_pos = {10,48}
  }
};

/* ************************************************************************** */
/*                                   Macros                                   */
/* ************************************************************************** */
#if 1
  #define SNOOKER_OBJ_BALL_SIZE                       (32)
  #define SNOOKER_OBJ_PLAYER_COLOR_ACTIVE             (lv_color_hex(0xFFFFFF))
  #define SNOOKER_OBJ_PLAYER_COLOR_INACTIVE           (lv_color_hex(0x646464))
  #define SNOOKER_OBJ_PLAYER_ICON_ROUNDED_RADIUS      (12)
  #define SNOOKER_OBJ_PLAYER_ICON_ARC_WIDTH           (2)
  #define SNOOKER_OBJ_PLAYER_HEAD_SIZE                (24)
  #define SNOOKER_OBJ_PLAYER_HEAD_POX_X               (0)
  #define SNOOKER_OBJ_PLAYER_HEAD_POX_Y               (-10)
  #define SNOOKER_OBJ_PLAYER_BODY_SIZE                (50)
  #define SNOOKER_OBJ_PLAYER_BODY_POS_X               (0)
  #define SNOOKER_OBJ_PLAYER_BODY_POS_Y               (30)
  #define SNOOKER_OBJ_PLAYER_BODY_ARC_0               (210)
  #define SNOOKER_OBJ_PLAYER_BODY_ARC_1               (330)
  #define SNOOKER_OBJ_SHADOW_COLOR                    (lv_color_hex(0x60FFE8))
  #define SNOOKER_OBJ_SHADOW_WIDTH                    (8)
  #define SNOOKER_OBJ_SHADOW_SPREAD                   (4)
  #define SNOOKER_OBJ_POINT_REMAINING_TXT_POS_X       (10)
  #define SNOOKER_OBJ_POINT_REMAINING_TXT_POS_Y       (-48)
#else
  #define SNOOKER_OBJ_BALL_SIZE                       (g_app_snooker_static.common.ballsize)
  #define SNOOKER_OBJ_PLAYER_COLOR_ACTIVE             (g_app_snooker_static.common.playercolor_active)
  #define SNOOKER_OBJ_PLAYER_COLOR_INACTIVE           (g_app_snooker_static.common.playercolor_inactive)
  #define SNOOKER_OBJ_PLAYER_ICON_ROUNDED_RADIUS      (g_app_snooker_static.common.player_icon_rounded_radius)
  #define SNOOKER_OBJ_PLAYER_HEAD_SIZE                (g_app_snooker_static.common.player_head_size)
  #define SNOOKER_OBJ_PLAYER_HEAD_POX_X               (g_app_snooker_static.common.player_head_pox[0])
  #define SNOOKER_OBJ_PLAYER_HEAD_POX_Y               (g_app_snooker_static.common.player_head_pox[1])
  #define SNOOKER_OBJ_PLAYER_BODY_SIZE                (g_app_snooker_static.common.player_body_size)
  #define SNOOKER_OBJ_PLAYER_BODY_POS_X               (g_app_snooker_static.common.player_body_pos[0])
  #define SNOOKER_OBJ_PLAYER_BODY_POS_Y               (g_app_snooker_static.common.player_body_pos[1])
  #define SNOOKER_OBJ_PLAYER_BODY_ARC_0               (g_app_snooker_static.common.player_body_arc[0])
  #define SNOOKER_OBJ_PLAYER_BODY_ARC_1               (g_app_snooker_static.common.player_body_arc[1])
  #define SNOOKER_OBJ_SHADOW_COLOR                    (g_app_snooker_static.common.shadow_color)
  #define SNOOKER_OBJ_SHADOW_WIDTH                    (g_app_snooker_static.common.shadow_width)
  #define SNOOKER_OBJ_SHADOW_SPREAD                   (g_app_snooker_static.common.shadow_spread)
  #define SNOOKER_OBJ_POINT_REMAINING_TXT_POS_X       (g_app_snooker_static.common.point_remain_txt_pos[0])
  #define SNOOKER_OBJ_POINT_REMAINING_TXT_POS_Y       (g_app_snooker_static.common.point_remain_txt_pos[1])
#endif
#define SNOOKER_OBJ_BALL_COLOR(idx)         (g_app_snooker_static.balls[(idx)].color)
#define SNOOKER_OBJ_BALL_POS_X(idx)         (g_app_snooker_static.balls[(idx)].pos[0])
#define SNOOKER_OBJ_BALL_POS_Y(idx)         (g_app_snooker_static.balls[(idx)].pos[1])
#define SNOOKER_OBJ_BALL_TEXT(idx)          (g_app_snooker_static.balls[(idx)].point)
#define SNOOKER_OBJ_DEFAULT_LIGHT_COLOR     lv_color_hex(0xFFFFFF)
#define SNOOKER_OBJ_DEFAULT_DARK_COLOR      lv_color_hex(0x000000)
#define SNOOKER_OBJ_NO_OPAQUE               (0xFF)
#define SNOOKER_OBJ_INVISIBLE               (0x00)
#define SNOOKER_OBJ_INITIAL_ACTIVE_BALL     (kSnookerBall_R)
#define SNOOKER_OBJ_INITIAL_ACTIVE_PLAYER   (kSnookerPlayer1)



/* ************************************************************************** */
/*                             Private Functions                              */
/* ************************************************************************** */

static void ui_snooker_draw_ball();

static void ui_snooker_draw_ballshadow();

static void ui_snooker_draw_players(lv_obj_t *p_screen) {
  for( int i = 0; i < kNumSnookerPlayer; ++i) {
    lv_obj_t *ui_player_score = lv_label_create(p_screen);
    {
      lv_obj_set_width           ( ui_player_score, LV_SIZE_CONTENT);  /// 1
      lv_obj_set_height          ( ui_player_score, LV_SIZE_CONTENT);   /// 1
      lv_obj_set_x               ( ui_player_score, g_app_snooker_static.players[i].pos[0] );
      lv_obj_set_y               ( ui_player_score, -10 );
      lv_obj_set_align           ( ui_player_score, LV_ALIGN_CENTER );
      lv_label_set_text          ( ui_player_score, g_app_snooker_dynamic.points[i].str);
      lv_obj_set_style_text_color( ui_player_score, SNOOKER_OBJ_DEFAULT_LIGHT_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_text_opa  ( ui_player_score, SNOOKER_OBJ_NO_OPAQUE          , LV_PART_MAIN | LV_STATE_DEFAULT );
    }

    lv_obj_t *ui_player = lv_obj_create(p_screen);
    {
      lv_obj_set_width             ( ui_player, SNOOKER_OBJ_PLAYER_BODY_SIZE);
      lv_obj_set_height            ( ui_player, SNOOKER_OBJ_PLAYER_BODY_SIZE);
      lv_obj_set_x                 ( ui_player, g_app_snooker_static.players[i].pos[0] );
      lv_obj_set_y                 ( ui_player, g_app_snooker_static.players[i].pos[1] );
      lv_obj_set_align             ( ui_player, LV_ALIGN_CENTER );
      lv_obj_clear_flag            ( ui_player, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
      lv_obj_set_style_radius      ( ui_player, SNOOKER_OBJ_PLAYER_ICON_ROUNDED_RADIUS, LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_bg_color    ( ui_player, SNOOKER_OBJ_DEFAULT_DARK_COLOR        , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_bg_opa      ( ui_player, SNOOKER_OBJ_INVISIBLE                 , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_border_color( ui_player, SNOOKER_OBJ_PLAYER_COLOR_INACTIVE     , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_border_opa  ( ui_player, SNOOKER_OBJ_NO_OPAQUE                 , LV_PART_MAIN | LV_STATE_DEFAULT );
    }

    lv_obj_t *ui_player_head = lv_obj_create(ui_player);
    {
      lv_obj_set_width             ( ui_player_head, SNOOKER_OBJ_PLAYER_HEAD_SIZE);
      lv_obj_set_height            ( ui_player_head, SNOOKER_OBJ_PLAYER_HEAD_SIZE);
      lv_obj_set_x                 ( ui_player_head, SNOOKER_OBJ_PLAYER_HEAD_POX_X);
      lv_obj_set_y                 ( ui_player_head, SNOOKER_OBJ_PLAYER_HEAD_POX_Y);
      lv_obj_set_align             ( ui_player_head, LV_ALIGN_CENTER );
      lv_obj_clear_flag            ( ui_player_head, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
      lv_obj_set_style_radius      ( ui_player_head, SNOOKER_OBJ_PLAYER_HEAD_SIZE      , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_bg_color    ( ui_player_head, SNOOKER_OBJ_PLAYER_COLOR_INACTIVE , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_bg_opa      ( ui_player_head, SNOOKER_OBJ_NO_OPAQUE             , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_border_color( ui_player_head, SNOOKER_OBJ_DEFAULT_DARK_COLOR    , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_border_opa  ( ui_player_head, SNOOKER_OBJ_INVISIBLE             , LV_PART_MAIN | LV_STATE_DEFAULT );
    }

    lv_obj_t *ui_player_body = lv_arc_create(ui_player);
    {
      lv_obj_set_width          ( ui_player_body, SNOOKER_OBJ_PLAYER_BODY_SIZE);
      lv_obj_set_height         ( ui_player_body, SNOOKER_OBJ_PLAYER_BODY_SIZE);
      lv_obj_set_x              ( ui_player_body, SNOOKER_OBJ_PLAYER_BODY_POS_X);
      lv_obj_set_y              ( ui_player_body, SNOOKER_OBJ_PLAYER_BODY_POS_Y);
      lv_obj_set_align          ( ui_player_body, LV_ALIGN_CENTER );
      lv_arc_set_value          ( ui_player_body, SNOOKER_OBJ_PLAYER_BODY_SIZE);
      lv_arc_set_bg_angles      ( ui_player_body, SNOOKER_OBJ_PLAYER_BODY_ARC_0, SNOOKER_OBJ_PLAYER_BODY_ARC_1);
      lv_obj_set_style_bg_color ( ui_player_body, SNOOKER_OBJ_DEFAULT_LIGHT_COLOR  , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_bg_opa   ( ui_player_body, SNOOKER_OBJ_INVISIBLE            , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_arc_color( ui_player_body, SNOOKER_OBJ_PLAYER_COLOR_INACTIVE, LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_arc_opa  ( ui_player_body, SNOOKER_OBJ_NO_OPAQUE            , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_arc_width( ui_player_body, SNOOKER_OBJ_PLAYER_ICON_ARC_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_arc_color( ui_player_body, SNOOKER_OBJ_DEFAULT_DARK_COLOR   , LV_PART_INDICATOR | LV_STATE_DEFAULT );
      lv_obj_set_style_arc_opa  ( ui_player_body, SNOOKER_OBJ_INVISIBLE            , LV_PART_INDICATOR | LV_STATE_DEFAULT );
      lv_obj_set_style_bg_color ( ui_player_body, SNOOKER_OBJ_DEFAULT_LIGHT_COLOR  , LV_PART_KNOB | LV_STATE_DEFAULT );
      lv_obj_set_style_bg_opa   ( ui_player_body, SNOOKER_OBJ_INVISIBLE            , LV_PART_KNOB | LV_STATE_DEFAULT );
    }

    if ( i == SNOOKER_OBJ_INITIAL_ACTIVE_PLAYER) {
      lv_obj_set_style_shadow_color (ui_player, SNOOKER_OBJ_SHADOW_COLOR , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_shadow_opa   (ui_player, SNOOKER_OBJ_NO_OPAQUE    , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_shadow_width (ui_player, SNOOKER_OBJ_SHADOW_WIDTH , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_shadow_spread(ui_player, SNOOKER_OBJ_SHADOW_SPREAD, LV_PART_MAIN | LV_STATE_DEFAULT );
    }
  }
}

static void ui_snooker_draw_playerscore(lv_obj_t *p_screen) {
  
}

static void ui_snooker_draw_rempoints(lv_obj_t *p_screen, const char *points_txt) {
  lv_obj_t *ui_remainingpts = lv_label_create(p_screen);
  lv_obj_set_width           ( ui_remainingpts, LV_SIZE_CONTENT);
  lv_obj_set_height          ( ui_remainingpts, LV_SIZE_CONTENT);
  lv_obj_set_x               ( ui_remainingpts, SNOOKER_OBJ_POINT_REMAINING_TXT_POS_X );
  lv_obj_set_y               ( ui_remainingpts, SNOOKER_OBJ_POINT_REMAINING_TXT_POS_Y );
  lv_obj_set_align           ( ui_remainingpts, LV_ALIGN_CENTER );
  lv_label_set_text_static   ( ui_remainingpts, points_txt);
  lv_obj_set_style_text_color( ui_remainingpts, SNOOKER_OBJ_DEFAULT_LIGHT_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_text_opa  ( ui_remainingpts, SNOOKER_OBJ_NO_OPAQUE          , LV_PART_MAIN | LV_STATE_DEFAULT );
}


static void ui_snooker_draw_rempoints_icon(lv_obj_t *p_screen) {
  lv_obj_t *ui_remainingicon = lv_arc_create(p_screen);
  lv_obj_set_width( ui_remainingicon, 20);
  lv_obj_set_height( ui_remainingicon, 20);
  lv_obj_set_x( ui_remainingicon, -17 );
  lv_obj_set_y( ui_remainingicon, -48 );
  lv_obj_set_align( ui_remainingicon, LV_ALIGN_CENTER );
  lv_arc_set_value(ui_remainingicon, 30);
  lv_arc_set_bg_angles(ui_remainingicon,270,629);
  lv_obj_set_style_border_color(ui_remainingicon, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_border_opa(ui_remainingicon, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_arc_color(ui_remainingicon, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_arc_opa(ui_remainingicon, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_arc_width(ui_remainingicon, 1, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_set_style_bg_color(ui_remainingicon, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_remainingicon, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
  lv_obj_set_style_arc_color(ui_remainingicon, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT );
  lv_obj_set_style_arc_opa(ui_remainingicon, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
  lv_obj_set_style_arc_width(ui_remainingicon, 10, LV_PART_INDICATOR| LV_STATE_DEFAULT);
  lv_obj_set_style_arc_rounded(ui_remainingicon, false, LV_PART_INDICATOR| LV_STATE_DEFAULT);

  lv_obj_set_style_bg_color(ui_remainingicon, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_remainingicon, 0, LV_PART_KNOB| LV_STATE_DEFAULT);
}



/* ************************************************************************** */
/*                              Public Functions                              */
/* ************************************************************************** */
void ui_snooker_screen_init(tAppSnooker *pApplication)
{
  lv_obj_t *p_screen = lv_obj_create(NULL);
  
  TRACE_DEBUG("Snooker APP: Initialize dynamic info");
  pApplication->_dynamic->p_screen = p_screen;
  pApplication->_dynamic->points[kSnookerPlayer1].val    = pApplication->_dynamic->points[kSnookerPlayer2].val    = 0;
  pApplication->_dynamic->points[kSnookerPlayer1].str[0] = pApplication->_dynamic->points[kSnookerPlayer2].str[0] = '0';
  pApplication->_dynamic->points[kSnookerPlayer1].str[1] = pApplication->_dynamic->points[kSnookerPlayer2].str[1] = '\0';
  pApplication->_dynamic->points_rem.val    = 147;
  pApplication->_dynamic->points_rem.str[0] = '1';
  pApplication->_dynamic->points_rem.str[1] = '4';
  pApplication->_dynamic->points_rem.str[2] = '7';
  pApplication->_dynamic->points_rem.str[3] = '\0';

  lv_obj_clear_flag        ( p_screen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_bg_color( p_screen, SNOOKER_OBJ_DEFAULT_DARK_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa  ( p_screen, SNOOKER_OBJ_NO_OPAQUE         , LV_PART_MAIN | LV_STATE_DEFAULT );

  TRACE_DEBUG("Snooker APP: Initialize balls ui");
  for( int i = 0; i < kNumSnookerBall; ++i) {
    lv_obj_t *ui_ball = lv_obj_create(p_screen);
    lv_obj_set_width             ( ui_ball, SNOOKER_OBJ_BALL_SIZE);
    lv_obj_set_height            ( ui_ball, SNOOKER_OBJ_BALL_SIZE);
    lv_obj_set_x                 ( ui_ball, SNOOKER_OBJ_BALL_POS_X(i) );
    lv_obj_set_y                 ( ui_ball, SNOOKER_OBJ_BALL_POS_Y(i) );
    lv_obj_set_align             ( ui_ball, LV_ALIGN_CENTER );
    lv_obj_clear_flag            ( ui_ball, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius      ( ui_ball, SNOOKER_OBJ_BALL_SIZE    , LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_color    ( ui_ball, SNOOKER_OBJ_BALL_COLOR(i), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa      ( ui_ball, SNOOKER_OBJ_NO_OPAQUE    , LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_border_width( ui_ball, SNOOKER_OBJ_INVISIBLE    , LV_PART_MAIN | LV_STATE_DEFAULT );

    if ( i == SNOOKER_OBJ_INITIAL_ACTIVE_BALL) {
      lv_obj_set_style_shadow_color ( ui_ball, SNOOKER_OBJ_SHADOW_COLOR , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_shadow_opa   ( ui_ball, SNOOKER_OBJ_NO_OPAQUE    , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_shadow_width ( ui_ball, SNOOKER_OBJ_SHADOW_WIDTH , LV_PART_MAIN | LV_STATE_DEFAULT );
      lv_obj_set_style_shadow_spread( ui_ball, SNOOKER_OBJ_SHADOW_SPREAD, LV_PART_MAIN | LV_STATE_DEFAULT );
    }

    lv_obj_t *ui_balltxt = lv_label_create(ui_ball);
    lv_obj_set_width           ( ui_balltxt, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height          ( ui_balltxt, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_align           ( ui_balltxt, LV_ALIGN_CENTER );
    lv_label_set_text_static   ( ui_balltxt, SNOOKER_OBJ_BALL_TEXT(i));

    if( i == kSnookerBall_Y || i == kSnookerBall_W) {
      lv_obj_set_style_text_color( ui_balltxt, SNOOKER_OBJ_DEFAULT_DARK_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT );
    }else {
      lv_obj_set_style_text_color( ui_balltxt, SNOOKER_OBJ_DEFAULT_LIGHT_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT );
    }
    lv_obj_set_style_text_opa( ui_balltxt, SNOOKER_OBJ_NO_OPAQUE, LV_PART_MAIN | LV_STATE_DEFAULT );
  }

#if 0
  lv_obj_t *ui_red = lv_obj_create(ui_snooker);
  lv_obj_set_width( ui_red, 32);
  lv_obj_set_height( ui_red, 32);
  lv_obj_set_x( ui_red, 0 );
  lv_obj_set_y( ui_red, -95 );
  lv_obj_set_align( ui_red, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_red, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_red, 32, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_red, lv_color_hex(0xBF0202), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_red, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(ui_red, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_color(ui_red, lv_color_hex(0x60FFE8), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_shadow_opa(ui_red, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_width(ui_red, 8, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_spread(ui_red, 4, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_redtxt = lv_label_create(ui_red);
  lv_obj_set_width( ui_redtxt, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height( ui_redtxt, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_align( ui_redtxt, LV_ALIGN_CENTER );
  lv_label_set_text(ui_redtxt,"1");
  lv_obj_set_style_text_color(ui_redtxt, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_text_opa(ui_redtxt, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_yellow = lv_obj_create(ui_snooker);
  lv_obj_set_width( ui_yellow, 32);
  lv_obj_set_height( ui_yellow, 32);
  lv_obj_set_x( ui_yellow, 48 );
  lv_obj_set_y( ui_yellow, -82 );
  lv_obj_set_align( ui_yellow, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_yellow, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_yellow, 32, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_yellow, lv_color_hex(0xFFC300), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_yellow, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(ui_yellow, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_yellowtxt = lv_label_create(ui_yellow);
  lv_obj_set_width( ui_yellowtxt, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height( ui_yellowtxt, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_align( ui_yellowtxt, LV_ALIGN_CENTER );
  lv_label_set_text(ui_yellowtxt,"2");

  lv_obj_t *ui_green = lv_obj_create(ui_snooker);
  lv_obj_set_width( ui_green, 32);
  lv_obj_set_height( ui_green, 32);
  lv_obj_set_x( ui_green, 82 );
  lv_obj_set_y( ui_green, -48 );
  lv_obj_set_align( ui_green, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_green, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_green, 32, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_green, lv_color_hex(0x297700), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_green, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(ui_green, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui_green, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_text_opa(ui_green, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_greentxt = lv_label_create(ui_green);
  lv_obj_set_width( ui_greentxt, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height( ui_greentxt, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_align( ui_greentxt, LV_ALIGN_CENTER );
  lv_label_set_text(ui_greentxt,"3");

  lv_obj_t *ui_brown = lv_obj_create(ui_snooker);
  lv_obj_set_width( ui_brown, 32);
  lv_obj_set_height( ui_brown, 32);
  lv_obj_set_x( ui_brown, 95 );
  lv_obj_set_y( ui_brown, 0 );
  lv_obj_set_align( ui_brown, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_brown, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_brown, 32, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_brown, lv_color_hex(0x7F3B03), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_brown, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(ui_brown, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui_brown, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_text_opa(ui_brown, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_browntxt = lv_label_create(ui_brown);
  lv_obj_set_width( ui_browntxt, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height( ui_browntxt, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_align( ui_browntxt, LV_ALIGN_CENTER );
  lv_label_set_text(ui_browntxt,"4");

  lv_obj_t *ui_blue = lv_obj_create(ui_snooker);
  lv_obj_set_width( ui_blue, 32);
  lv_obj_set_height( ui_blue, 32);
  lv_obj_set_x( ui_blue, -95 );
  lv_obj_set_y( ui_blue, 0 );
  lv_obj_set_align( ui_blue, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_blue, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_blue, 32, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_blue, lv_color_hex(0x0800F8), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_blue, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(ui_blue, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui_blue, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_text_opa(ui_blue, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_bluetxt = lv_label_create(ui_blue);
  lv_obj_set_width( ui_bluetxt, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height( ui_bluetxt, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_align( ui_bluetxt, LV_ALIGN_CENTER );
  lv_label_set_text(ui_bluetxt,"5");

  lv_obj_t *ui_pink = lv_obj_create(ui_snooker);
  lv_obj_set_width( ui_pink, 32);
  lv_obj_set_height( ui_pink, 32);
  lv_obj_set_x( ui_pink, -82 );
  lv_obj_set_y( ui_pink, -48 );
  lv_obj_set_align( ui_pink, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_pink, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_pink, 32, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_pink, lv_color_hex(0xF800E3), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_pink, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(ui_pink, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui_pink, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_text_opa(ui_pink, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_pinktxt = lv_label_create(ui_pink);
  lv_obj_set_width( ui_pinktxt, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height( ui_pinktxt, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_align( ui_pinktxt, LV_ALIGN_CENTER );
  lv_label_set_text(ui_pinktxt,"6");

  lv_obj_t *ui_black = lv_obj_create(ui_snooker);
  lv_obj_set_width( ui_black, 32);
  lv_obj_set_height( ui_black, 32);
  lv_obj_set_x( ui_black, -48 );
  lv_obj_set_y( ui_black, -82 );
  lv_obj_set_align( ui_black, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_black, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_black, 32, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_black, lv_color_hex(0x464646), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_black, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(ui_black, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(ui_black, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_text_opa(ui_black, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_blacktxt = lv_label_create(ui_black);
  lv_obj_set_width( ui_blacktxt, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height( ui_blacktxt, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_align( ui_blacktxt, LV_ALIGN_CENTER );
  lv_label_set_text(ui_blacktxt,"7");

  lv_obj_t *ui_white = lv_obj_create(ui_snooker);
  lv_obj_set_width( ui_white, 32);
  lv_obj_set_height( ui_white, 32);
  lv_obj_set_x( ui_white, 0 );
  lv_obj_set_y( ui_white, 90 );
  lv_obj_set_align( ui_white, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_white, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_white, 32, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_white, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_white, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(ui_white, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_color(ui_white, lv_color_hex(0x60FFE8), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_shadow_opa(ui_white, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_width(ui_white, 8, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_spread(ui_white, 4, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_whitetxt = lv_label_create(ui_white);
  lv_obj_set_width( ui_whitetxt, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height( ui_whitetxt, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_align( ui_whitetxt, LV_ALIGN_CENTER );
  lv_label_set_text(ui_whitetxt,"-4");
#endif

  TRACE_DEBUG("Snooker APP: Initialize players ui");
  ui_snooker_draw_players(p_screen);

#if 0
  lv_obj_t *ui_player2 = lv_obj_create(ui_snooker);
  lv_obj_set_width( ui_player2, 50);
  lv_obj_set_height( ui_player2, 50);
  lv_obj_set_x( ui_player2, 30 );
  lv_obj_set_y( ui_player2, 30 );
  lv_obj_set_align( ui_player2, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_player2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_player2, 12, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_player2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_player2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_color(ui_player2, lv_color_hex(0x00CCD9), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_shadow_opa(ui_player2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_width(ui_player2, 8, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_shadow_spread(ui_player2, 4, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_player2head = lv_obj_create(ui_player2);
  lv_obj_set_width( ui_player2head, 24);
  lv_obj_set_height( ui_player2head, 24);
  lv_obj_set_x( ui_player2head, 0 );
  lv_obj_set_y( ui_player2head, -10 );
  lv_obj_set_align( ui_player2head, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_player2head, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_player2head, 20, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_player2body = lv_arc_create(ui_player2);
  lv_obj_set_width( ui_player2body, 50);
  lv_obj_set_height( ui_player2body, 50);
  lv_obj_set_x( ui_player2body, 0 );
  lv_obj_set_y( ui_player2body, 30 );
  lv_obj_set_align( ui_player2body, LV_ALIGN_CENTER );
  lv_arc_set_value(ui_player2body, 50);
  lv_arc_set_bg_angles(ui_player2body,210,330);
  lv_obj_set_style_arc_width(ui_player2body, 2, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_set_style_arc_color(ui_player2body, lv_color_hex(0x4040FF), LV_PART_INDICATOR | LV_STATE_DEFAULT );
  lv_obj_set_style_arc_opa(ui_player2body, 0, LV_PART_INDICATOR| LV_STATE_DEFAULT);

  lv_obj_set_style_bg_color(ui_player2body, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_player2body, 0, LV_PART_KNOB| LV_STATE_DEFAULT);

  lv_obj_t *ui_player2score = lv_label_create(ui_snooker);
  lv_obj_set_width( ui_player2score, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height( ui_player2score, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_x( ui_player2score, 26 );
  lv_obj_set_y( ui_player2score, -10 );
  lv_obj_set_align( ui_player2score, LV_ALIGN_CENTER );
  lv_label_set_text(ui_player2score,"147");
  lv_obj_set_style_text_color(ui_player2score, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_text_opa(ui_player2score, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
#endif
  TRACE_DEBUG("Snooker APP: Initialize icons ui");
  ui_snooker_draw_rempoints_icon(p_screen);
#if 0
  lv_obj_t *ui_remainingicon = lv_arc_create(ui_snooker);
  lv_obj_set_width( ui_remainingicon, 20);
  lv_obj_set_height( ui_remainingicon, 20);
  lv_obj_set_x( ui_remainingicon, -17 );
  lv_obj_set_y( ui_remainingicon, -48 );
  lv_obj_set_align( ui_remainingicon, LV_ALIGN_CENTER );
  lv_arc_set_value(ui_remainingicon, 30);
  lv_arc_set_bg_angles(ui_remainingicon,270,629);
  lv_obj_set_style_border_color(ui_remainingicon, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_border_opa(ui_remainingicon, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_arc_color(ui_remainingicon, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_arc_opa(ui_remainingicon, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_arc_width(ui_remainingicon, 1, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_set_style_bg_color(ui_remainingicon, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_remainingicon, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
  lv_obj_set_style_arc_color(ui_remainingicon, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT );
  lv_obj_set_style_arc_opa(ui_remainingicon, 255, LV_PART_INDICATOR| LV_STATE_DEFAULT);
  lv_obj_set_style_arc_width(ui_remainingicon, 10, LV_PART_INDICATOR| LV_STATE_DEFAULT);
  lv_obj_set_style_arc_rounded(ui_remainingicon, false, LV_PART_INDICATOR| LV_STATE_DEFAULT);

  lv_obj_set_style_bg_color(ui_remainingicon, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_remainingicon, 0, LV_PART_KNOB| LV_STATE_DEFAULT);
#endif
  
  ui_snooker_draw_rempoints(p_screen, pApplication->_dynamic->points_rem.str);
#if 0
  lv_obj_t *ui_remainingpts = lv_label_create(ui_snooker);
  lv_obj_set_width( ui_remainingpts, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height( ui_remainingpts, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_x( ui_remainingpts, 10 );
  lv_obj_set_y( ui_remainingpts, -48 );
  lv_obj_set_align( ui_remainingpts, LV_ALIGN_CENTER );
  lv_label_set_text(ui_remainingpts,"147");
  lv_obj_set_style_text_color(ui_remainingpts, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_text_opa(ui_remainingpts, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
#endif
  lv_scr_load(p_screen);
}


void ui_snooker_switch_player(void) {

}

void ui_snooker_switch_ball(void) {

}

void ui_snooker_update_remaining(void) {

}

/**
 * @brief Clock Application Main Entrance
 * @param [in] param - will cast to `tAppClock*`
 */
void app_snooker_main(void *param) RTOSTHREAD {
#define CAST(x) ((tAppSnooker*)(x))
  ui_snooker_screen_init(CAST(param));
  while(1){
    vTaskDelay(100);
  }
#undef CAST
}