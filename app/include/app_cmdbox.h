/**
 ******************************************************************************
 * @file    app_cmdbox.h
 * @author  RandleH
 * @brief   Application Program - Command Box
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
#ifndef APP_CMDBOX_H
#define APP_CMDBOX_H


/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include "cmn_type.h"
#include "app_type.h"
#include "bsp_type.h"



/* ************************************************************************** */
/*                              Public Macros                                 */
/* ************************************************************************** */
#define MAX_NUN_ARGS_SUPPORTED      8
#define MAX_NUM_PENDING             2
#define MAX_CMD_STRING_LEN          BSP_CFG_UART_RX_BUF_SIZE

#ifdef __cplusplus
extern "C"{
#endif

/* ************************************************************************** */
/*                              Private Objects                               */
/* ************************************************************************** */
typedef int arg_t;

typedef struct stAppCmdboxDatabaseListUnit {
  const char *keyword;
  int       (*callback)(const char *, ...);
  size_t     nargs;
} tAppCmdboxDatabaseListUnit;

typedef struct stAppCmdboxDatabaseList{
  const tAppCmdboxDatabaseListUnit *database;
  const size_t                     len;
} tAppCmdboxDatabaseList;

typedef struct stAppCmdBoxPendingExe {
  arg_t                      args[MAX_NUN_ARGS_SUPPORTED];
  const tAppCmdboxDatabaseListUnit *p_matched_cmd;
} tAppCmdBoxPendingExe;

typedef struct stAppCmdBox {
  char                 cmd_buf[BSP_CFG_UART_RX_BUF_SIZE];
  tAppCmdBoxPendingExe pending_exe[MAX_NUM_PENDING];
} tAppCmdBox;

#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || (defined EMULATOR_STM32F405RGT6)
void app_cmdbox_main(void *param) RTOSTHREAD;
void app_cmdbox_idle(void *param) RTOSIDLE;
#endif
void app_cmdbox_parse(tAppCmdBox *p_cmdbox, const char *cmd);

#ifdef __cplusplus
}
#endif



#endif
/* ********************************** EOF *********************************** */
