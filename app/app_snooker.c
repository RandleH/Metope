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

static void ui_snooker_draw_balls(lv_obj_t *p_screen) {
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
}

static void ui_snooker_draw_ballshadow();

static void ui_snooker_draw_players(lv_obj_t *p_screen) {
  for( int i = 0; i < kNumSnookerPlayer; ++i) {

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
  }
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

  TRACE_DEBUG("Snooker APP: Initialize screen canvas");
  lv_obj_clear_flag        ( p_screen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_bg_color( p_screen, SNOOKER_OBJ_DEFAULT_DARK_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa  ( p_screen, SNOOKER_OBJ_NO_OPAQUE         , LV_PART_MAIN | LV_STATE_DEFAULT );

  TRACE_DEBUG("Snooker APP: Initialize balls ui");
  ui_snooker_draw_balls(p_screen);

  TRACE_DEBUG("Snooker APP: Initialize players ui");
  ui_snooker_draw_players(p_screen);
  ui_snooker_draw_playerscore(p_screen);

  TRACE_DEBUG("Snooker APP: Initialize icons ui");
  ui_snooker_draw_rempoints_icon(p_screen);
  
  TRACE_DEBUG("Snooker APP: Initialize remaining points ui");
  ui_snooker_draw_rempoints(p_screen, pApplication->_dynamic->points_rem.str);

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