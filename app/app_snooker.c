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
#include "lvgl.h"
#include "app_lvgl.h"
#include "app_clock.h"
#include "cmn_utility.h"
#include "cmn_color.h"


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
  kNumSnookerBall
} AppSnookerBall_t;

typedef struct stAppSnookerBallStatic {
  lv_color_t color;
  const char *point;
} tAppSnookerBallStatic;


typedef struct stAppSnookerBallDynamic {
  lv_obj_t     *p_obj;
  cmnBoolean_t is_selected;
} tAppSnookerBallDynamic;



typedef struct stAppSnooker {
  char    points[2+1][3+1];
  uint8_t selected_ball;
  uint8_t selected_player;
  uint8_t last_operation;

  tAppSnookerBallDynamic *_dynamic;
  const tAppSnookerBallStatic  *_static;
} tAppSnooker;


const tAppSnookerBallStatic ball_info_static[kNumSnookerBall] = {
  [kSnookerBall_R  ] = {.color = lv_color_hex(0xBF0202), .point = "1"},
  [kSnookerBall_Y  ] = {.color = lv_color_hex(0x??????), .point = "2"},
  [kSnookerBall_G  ] = {.color = lv_color_hex(0x??????), .point = "3"},
  [kSnookerBall_BRN] = {.color = lv_color_hex(0x??????), .point = "4"},
  [kSnookerBall_BLU] = {.color = lv_color_hex(0x??????), .point = "5"},
  [kSnookerBall_P  ] = {.color = lv_color_hex(0x??????), .point = "6"},
  [kSnookerBall_BLK] = {.color = lv_color_hex(0x??????), .point = "7"}
};


void ui_snooker_screen_init(void)
{
  lv_obj_t *ui_snooker = lv_obj_create(NULL);
  lv_obj_clear_flag( ui_snooker, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_bg_color(ui_snooker, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_snooker, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

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

  lv_obj_t *ui_player1score = lv_label_create(ui_snooker);
  lv_obj_set_width( ui_player1score, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height( ui_player1score, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_x( ui_player1score, -31 );
  lv_obj_set_y( ui_player1score, -10 );
  lv_obj_set_align( ui_player1score, LV_ALIGN_CENTER );
  lv_label_set_text(ui_player1score,"147");
  lv_obj_set_style_text_color(ui_player1score, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_text_opa(ui_player1score, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_player1 = lv_obj_create(ui_snooker);
  lv_obj_set_width( ui_player1, 50);
  lv_obj_set_height( ui_player1, 50);
  lv_obj_set_x( ui_player1, -30 );
  lv_obj_set_y( ui_player1, 30 );
  lv_obj_set_align( ui_player1, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_player1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_player1, 12, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_player1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_player1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_color(ui_player1, lv_color_hex(0x646464), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_border_opa(ui_player1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_player1head = lv_obj_create(ui_player1);
  lv_obj_set_width( ui_player1head, 24);
  lv_obj_set_height( ui_player1head, 24);
  lv_obj_set_x( ui_player1head, 0 );
  lv_obj_set_y( ui_player1head, -10 );
  lv_obj_set_align( ui_player1head, LV_ALIGN_CENTER );
  lv_obj_clear_flag( ui_player1head, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
  lv_obj_set_style_radius(ui_player1head, 20, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(ui_player1head, lv_color_hex(0x646464), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_player1head, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_border_color(ui_player1head, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_border_opa(ui_player1head, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_t *ui_player1body = lv_arc_create(ui_player1);
  lv_obj_set_width( ui_player1body, 50);
  lv_obj_set_height( ui_player1body, 50);
  lv_obj_set_x( ui_player1body, 0 );
  lv_obj_set_y( ui_player1body, 30 );
  lv_obj_set_align( ui_player1body, LV_ALIGN_CENTER );
  lv_arc_set_value(ui_player1body, 50);
  lv_arc_set_bg_angles(ui_player1body,210,330);
  lv_obj_set_style_bg_color(ui_player1body, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_player1body, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_arc_color(ui_player1body, lv_color_hex(0x646464), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_arc_opa(ui_player1body, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
  lv_obj_set_style_arc_width(ui_player1body, 2, LV_PART_MAIN| LV_STATE_DEFAULT);

  lv_obj_set_style_arc_color(ui_player1body, lv_color_hex(0x4040FF), LV_PART_INDICATOR | LV_STATE_DEFAULT );
  lv_obj_set_style_arc_opa(ui_player1body, 0, LV_PART_INDICATOR| LV_STATE_DEFAULT);

  lv_obj_set_style_bg_color(ui_player1body, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT );
  lv_obj_set_style_bg_opa(ui_player1body, 0, LV_PART_KNOB| LV_STATE_DEFAULT);

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

  lv_obj_t *ui_remainingpts = lv_label_create(ui_snooker);
  lv_obj_set_width( ui_remainingpts, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height( ui_remainingpts, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_x( ui_remainingpts, 10 );
  lv_obj_set_y( ui_remainingpts, -48 );
  lv_obj_set_align( ui_remainingpts, LV_ALIGN_CENTER );
  lv_label_set_text(ui_remainingpts,"147");
  lv_obj_set_style_text_color(ui_remainingpts, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
  lv_obj_set_style_text_opa(ui_remainingpts, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

}


void ui_snooker_switch_player(void) {

}

void ui_snooker_switch_ball(void) {

}

void ui_snooker_update_remaining(void) {

}