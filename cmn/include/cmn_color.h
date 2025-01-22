/**
 ******************************************************************************
 * @file    cmn_color.h
 * @author  RandleH
 * @brief   Color RGB / Grayscale / Binary
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 RandleH.
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
/*                             Interface Includes                             */
/* ************************************************************************** */
#include "lvgl.h"


#ifndef CMN_COLOR_H
#define CMN_COLOR_H




/* ************************************************************************** */
/*                               Interface Macro                              */
/* ************************************************************************** */
#define MAKE_COLOR_255(R_255, G_255, B_255) LV_COLOR_MAKE(R_255, G_255, B_255) //...//
#define GET_R(RGB)                          LV_COLOR_GET_R(RGB) //...//
#define GET_G(RGB)                          LV_COLOR_GET_G(RGB) //...//
#define GET_B(RGB)                          LV_COLOR_GET_B(RGB) //...//

/* ************************************************************************** */
/*                               Interface Macro                              */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

typedef lv_color_t cmnColor_t;

/* ************************************************************************** */
/*                             Interface Function                             */
/* ************************************************************************** */
static inline cmnColor_t cmn_color_make(uint8_t r, uint8_t g, uint8_t b);
static inline cmnColor_t cmn_color_make(uint8_t r, uint8_t g, uint8_t b){
  cmnColor_t rgb = {
    .ch = {
      .green_h = g>>3,
      .red     = r,
      .blue    = g,
      .green_l = g&0x07
    }
  };
  return rgb;
}

#ifdef __cplusplus
}
#endif // __cplusplus


/* ************************************************************************** */
/*                                  Palette                                   */
/* ************************************************************************** */
// Standard
#define M_COLOR_WHITE                        (MAKE_COLOR_255(255,255,255))  // 白色
#define M_COLOR_BLACK                        (MAKE_COLOR_255(  0,  0,  0))  // 黑色

#define M_COLOR_BLUE                         (MAKE_COLOR_255(  0,  0,255))  // 蓝色
#define M_COLOR_RED                          (MAKE_COLOR_255(255,  0,  0))  // 红色
#define M_COLOR_GREEN                        (MAKE_COLOR_255(  0,255,  0))  // 绿色
#define M_COLOR_YELLOW                       (MAKE_COLOR_255(255,255,  0))  // 黄色
#define M_COLOR_CYAN                         (MAKE_COLOR_255(  0,255,255))  // 青色
#define M_COLOR_MAGENTA                      (MAKE_COLOR_255(255,  0,255))  // 洋紫

// Red-Blue Series           
#define M_COLOR_PINK                         (MAKE_COLOR_255(255,192,203))  // 粉红
#define M_COLOR_CRIMSON                      (MAKE_COLOR_255(220, 20, 60))  // 猩红
#define M_COLOR_LAVENDERBLUSH                (MAKE_COLOR_255(255,240,245))  // 苍白紫罗兰红
#define M_COLOR_PALEVIOLATRED                (MAKE_COLOR_255(219,112,147))  // 羞涩淡紫红
#define M_COLOR_HOTPINK                      (MAKE_COLOR_255(255,105,180))  // 热情粉红
#define M_COLOR_MEDIUMVIOLATRED              (MAKE_COLOR_255(199, 21,133))  // 适中紫罗兰
#define M_COLOR_ORCHID                       (MAKE_COLOR_255(218,112,214))  // 兰花紫
#define M_COLOR_THISTLE                      (MAKE_COLOR_255(216,191,216))  // 苍紫
#define M_COLOR_PLUM                         (MAKE_COLOR_255(221,160,221))  // 轻紫
#define M_COLOR_VOILET                       (MAKE_COLOR_255(218,112,214))  // 紫罗兰
#define M_COLOR_DARKVOILET                   (MAKE_COLOR_255(255,  0,255))  // 紫红
#define M_COLOR_PURPLE                       (MAKE_COLOR_255(128,  0,128))  // 紫
#define M_COLOR_MEDIUMORCHID                 (MAKE_COLOR_255(255,  0,255))  // 适中兰花紫
#define M_COLOR_DARKVIOLET                   (MAKE_COLOR_255(148,  0,211))  // 深紫罗兰
#define M_COLOR_INDIGO                       (MAKE_COLOR_255( 75,  0,130))  // 靓青
#define M_COLOR_BLUEVIOLET                   (MAKE_COLOR_255(138, 43,226))  // 蓝紫罗兰
#define M_COLOR_MEDIUMPURPLE                 (MAKE_COLOR_255(147,112,219))  // 适中紫
#define M_COLOR_MEDIUMSLATEBLUE              (MAKE_COLOR_255(123,104,238))  // 适中板岩蓝
#define M_COLOR_SLATEBLUE                    (MAKE_COLOR_255(106, 90,205))  // 板岩蓝
#define M_COLOR_DARKSLATEBLUE                (MAKE_COLOR_255( 72, 61,139))  // 深板岩蓝
#define M_COLOR_LAVENDER                     (MAKE_COLOR_255(230,230,250))  // 薰衣草淡
#define M_COLOR_GHOSTWHITE                   (MAKE_COLOR_255(248,248,255))  // 幽灵白

// Blue-Green Series           
#define M_COLOR_MEDIUMBLUE                   (MAKE_COLOR_255(  0,  0,205))  // 适中蓝
#define M_COLOR_MIDNIGHTBLUE                 (MAKE_COLOR_255( 25, 25,112))  // 午夜蓝
#define M_COLOR_DARKBLUE                     (MAKE_COLOR_255(  0,  0,139))  // 深蓝
#define M_COLOR_NAVY                         (MAKE_COLOR_255(  0,  0,128))  // 海军蓝
#define M_COLOR_ROYALBLUE                    (MAKE_COLOR_255( 65,105,225))  // 皇家蓝
#define M_COLOR_CORNFLOWERBLUE               (MAKE_COLOR_255(100,149,237))  // 矢车菊蓝
#define M_COLOR_LIGHTSTEELBLUE               (MAKE_COLOR_255(176,196,222))  // 淡钢蓝
#define M_COLOR_LIGHTSLATEGRAY               (MAKE_COLOR_255(119,136,153))  // 浅板岩灰
#define M_COLOR_SLATEGRAY                    (MAKE_COLOR_255(112,128,144))  // 石板灰
#define M_COLOR_DODGERBLUE                   (MAKE_COLOR_255( 30,114,255))  // 道奇蓝
#define M_COLOR_ALICEBLUE                    (MAKE_COLOR_255(240,248,255))  // 爱丽丝蓝
#define M_COLOR_STEELBLUE                    (MAKE_COLOR_255( 70,130,180))  // 钢蓝
#define M_COLOR_LIGHTSKYBLUE                 (MAKE_COLOR_255(135,206,250))  // 淡天蓝
#define M_COLOR_SKYBLUE                      (MAKE_COLOR_255(135,206,235))  // 天蓝
#define M_COLOR_DEEPSKYBLUE                  (MAKE_COLOR_255(  0,191,255))  // 深天蓝
#define M_COLOR_LIGHTBLUE                    (MAKE_COLOR_255(173,216,230))  // 淡蓝
#define M_COLOR_POWDERBLUE                   (MAKE_COLOR_255(176,224,230))  // 火药蓝
#define M_COLOR_CADETBLUE                    (MAKE_COLOR_255( 95,158,160))  // 军校蓝
#define M_COLOR_AZURE                        (MAKE_COLOR_255(245,255,255))  // 蔚蓝
#define M_COLOR_LIGHTCYAN                    (MAKE_COLOR_255(240,255,255))  // 淡青
#define M_COLOR_PALETURQUOISE                (MAKE_COLOR_255(175,238,238))  // 苍白宝石绿
#define M_COLOR_AQUA                         (MAKE_COLOR_255(  0,255,255))  // 水绿
#define M_COLOR_DARKTURQUOISE                (MAKE_COLOR_255(  0,206,209))  // 深宝石绿
#define M_COLOR_DARKSLATEGRAY                (MAKE_COLOR_255( 47, 79, 79))  // 深石板灰
#define M_COLOR_DARKCYAN                     (MAKE_COLOR_255(  0,139,139))  // 深青色
#define M_COLOR_TEAL                         (MAKE_COLOR_255(  0,128,128))  // 水鸭色
#define M_COLOR_MEDIUMTURQUOISE              (MAKE_COLOR_255( 72,209,204))  // 适中宝石绿
#define M_COLOR_LIGHTSEEGREEN                (MAKE_COLOR_255( 32,178,170))  // 浅海样绿
#define M_COLOR_TURQUOISE                    (MAKE_COLOR_255( 64,224,208))  // 宝石绿
#define M_COLOR_AQUAMARINE                   (MAKE_COLOR_255(127,255,212))  // 碧绿
#define M_COLOR_MEDIUMAQUAMARINE             (MAKE_COLOR_255(102,205,170))  // 适中碧绿
#define M_COLOR_MEDIUMSPRINGGREEN            (MAKE_COLOR_255(  0,250,154))  // 适中春天绿
#define M_COLOR_SPRINGGREEN                  (MAKE_COLOR_255(  0,255,127))  // 春天绿
#define M_COLOR_MEDIUMSEEGREEN               (MAKE_COLOR_255( 60,179,113))  // 适中海洋绿
#define M_COLOR_SEEGREEN                     (MAKE_COLOR_255( 46,139, 87))  // 海洋绿
#define M_COLOR_LIGHTGREEN                   (MAKE_COLOR_255(144,238,144))  // 浅绿
#define M_COLOR_PALEGREEN                    (MAKE_COLOR_255(152,251,152))  // 苍白绿
#define M_COLOR_DARKSEEGREEN                 (MAKE_COLOR_255(143,188,143))  // 深海洋绿
#define M_COLOR_LIME                         (MAKE_COLOR_255( 50,205, 50))  // 莱姆色
#define M_COLOR_CHARTREUSE                   (MAKE_COLOR_255(127,255,  0))  // 查特酒绿

// Green-RED Series           
#define M_COLOR_FORESTGREEN                  (MAKE_COLOR_255( 34,139, 34))  // 森林绿
#define M_COLOR_LAWNGREEN                    (MAKE_COLOR_255(124,252,  0))  // 草坪绿
#define M_COLOR_GREENYELLOW                  (MAKE_COLOR_255(173,255, 47))  // 绿黄
#define M_COLOR_DARKOLIVEGREEN               (MAKE_COLOR_255( 85,107, 47))  // 深橄榄绿
#define M_COLOR_YELLOWGREEN                  (MAKE_COLOR_255(154,205, 50))  // 黄绿
#define M_COLOR_OLIVEDRAB                    (MAKE_COLOR_255( 34,139, 34))  // 橄榄褐
#define M_COLOR_BEIGE                        (MAKE_COLOR_255(245,245,220))  // 米色
#define M_COLOR_LIGHTRODYELLOW               (MAKE_COLOR_255( 34,139, 34))  // 浅秋黄
#define M_COLOR_IVORY                        (MAKE_COLOR_255(255,255,240))  // 象牙白
#define M_COLOR_OLIVE                        (MAKE_COLOR_255(128,128,  0))  // 橄榄
#define M_COLOR_DARKKHAKI                    (MAKE_COLOR_255(189,183,107))  // 深卡其布
#define M_COLOR_LEMONCHIFFON                 (MAKE_COLOR_255(255,250,205))  // 柠檬沙
#define M_COLOR_PALEGOLDROD                  (MAKE_COLOR_255(238,232,170))  // 灰秋
#define M_COLOR_KHAKI                        (MAKE_COLOR_255(240,230,140))  // 卡其布
#define M_COLOR_GOLDEN                       (MAKE_COLOR_255(255,215,  0))  // 金色
#define M_COLOR_CORNMILK                     (MAKE_COLOR_255(255,248,220))  // 玉米
#define M_COLOR_GOLDROD                      (MAKE_COLOR_255(218,165, 32))  // 秋天
#define M_COLOR_DARKGOLDROD                  (MAKE_COLOR_255(184,134, 11))  // 深秋
#define M_COLOR_FLORALWHITE                  (MAKE_COLOR_255(255,250,240))  // 白花
#define M_COLOR_OLDLACE                      (MAKE_COLOR_255(253,245,230))  // 浅米色
#define M_COLOR_WHEAT                        (MAKE_COLOR_255(245,222,179))  // 小麦
#define M_COLOR_MOCCASIN                     (MAKE_COLOR_255(255,228,181))  // 鹿皮
#define M_COLOR_ORANGE                       (MAKE_COLOR_255(255,165,  0))  // 橙色
#define M_COLOR_PAPAYAWHIP                   (MAKE_COLOR_255(255,239,213))  // 木瓜
#define M_COLOR_BLANCHEDALMOND               (MAKE_COLOR_255(255,235,205))  // 漂白的杏仁
#define M_COLOR_NAVAJOWHITE                  (MAKE_COLOR_255(255,222,173))  // 耐而节白
#define M_COLOR_ANTIQUEWHITE                 (MAKE_COLOR_255(250,235,215))  // 古白
#define M_COLOR_TAN                          (MAKE_COLOR_255(210,180,140))  // 晒
#define M_COLOR_BURLYWOOD                    (MAKE_COLOR_255(222,184,135))  // 树干
#define M_COLOR_BISQUE                       (MAKE_COLOR_255(255,228,196))  // 乳脂
#define M_COLOR_DARKORANGE                   (MAKE_COLOR_255(255,140,  0))  // 深橙色
#define M_COLOR_LINEN                        (MAKE_COLOR_255(255,240,230))  // 亚麻
#define M_COLOR_PERU                         (MAKE_COLOR_255(205,133, 63))  // 秘鲁
#define M_COLOR_SANDYBROWN                   (MAKE_COLOR_255(244,164, 96))  // 沙棕
#define M_COLOR_CHOCOLATE                    (MAKE_COLOR_255(210,105, 30))  // 巧克力
#define M_COLOR_SEASHELL                     (MAKE_COLOR_255(255,245,238))  // 海贝
#define M_COLOR_SIENNA                       (MAKE_COLOR_255(160, 82, 45))  // 土黄赭
#define M_COLOR_SALMON                       (MAKE_COLOR_255(255,160,122))  // 三文鱼
#define M_COLOR_CORAL                        (MAKE_COLOR_255(255,127, 80))  // 珊瑚红
#define M_COLOR_ORANGERED                    (MAKE_COLOR_255(255, 69,  0))  // 橙红
#define M_COLOR_TOMATO                       (MAKE_COLOR_255(255, 99, 71))  // 番茄
#define M_COLOR_MISTYROSE                    (MAKE_COLOR_255(255,228,225))  // 迷雾玫瑰
#define M_COLOR_BLOODYMEAT                   (MAKE_COLOR_255(250,128,114))  // 鲜肉
#define M_COLOR_LIGHTCORAL                   (MAKE_COLOR_255(240,128,128))  // 浅珊瑚红
#define M_COLOR_ROSEBROWN                    (MAKE_COLOR_255(188,143,143))  // 玫瑰棕
#define M_COLOR_INDIANRED                    (MAKE_COLOR_255(205, 92, 92))  // 浅粉红
#define M_COLOR_BROWN                        (MAKE_COLOR_255(165, 42, 42))  // 棕色
#define M_COLOR_FIREBRICK                    (MAKE_COLOR_255(178, 34, 34))  // 火砖
#define M_COLOR_DARKRED                      (MAKE_COLOR_255(139,  0,  0))  // 深红
#define M_COLOR_MAROON                       (MAKE_COLOR_255(128,  0,  0))  // 栗色

// Neutral Series           
#define M_COLOR_WHITESMOKE                   (MAKE_COLOR_255(245,245,245))  // 烟白
#define M_COLOR_GAINSBORO                    (MAKE_COLOR_255(220,220,220))  // 赶死部落
#define M_COLOR_LIGHTGRAY                    (MAKE_COLOR_255(211,211,211))  // 浅灰
#define M_COLOR_SILVER                       (MAKE_COLOR_255(192,192,192))  // 银色
#define M_COLOR_DARKGRAY                     (MAKE_COLOR_255( 73, 73, 73))  // 深灰
#define M_COLOR_DIMGRAY                      (MAKE_COLOR_255( 54, 54, 54))  // 暗灰

#define M_COLOR_COAL                         (MAKE_COLOR_255( 34, 35, 34))  // 煤炭黑




/* ************************************************************************** */
/*                               Public Function                              */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif

cmnColor_t cmn_color_gradient( cmnColor_t color_from, cmnColor_t color_to, uint8_t x);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // CMN_COLOR_H

/* ********************************** EOF *********************************** */
