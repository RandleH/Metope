/**
 ******************************************************************************
 * @file    app_cmdbox.c
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

/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <ctype.h>
#include "assert.h"
#include "trace.h"
#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || (defined EMULATOR_STM32F405RGT6)
  #include "global.h"
  #include "FreeRTOS.h"
#endif
#include "bsp_type.h"
#include "app_cmdbox.h"
#include "app_cmdbox_callback.h" /*!< This must be included at the end of the header file list */

#ifdef __cplusplus
extern "C" {
#endif


/* ************************************************************************** */
/*                              Private Macros                                */
/* ************************************************************************** */
#define MAX_NUN_ARGS_SUPPORTED      8


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


/* ************************************************************************** */
/*                             Private Variables                              */
/* ************************************************************************** */
static const tAppCmdboxDatabaseList CMD_DUMMY = {
  .database = NULL,
  .len      = 0
};

static const tAppCmdboxDatabaseListUnit CMD_C_LIST[] = {
  {
    .keyword  = "CCW",
    .callback = app_cmdbox_callback_1args_CCW,
    .nargs    = 1
  },
  {
    .keyword  = "CW",
    .callback = app_cmdbox_callback_1args_CW,
    .nargs    = 1
  }
};
static const tAppCmdboxDatabaseList CMD_C = {
  .database = &CMD_C_LIST[0],
  .len      = sizeof(CMD_C_LIST)/sizeof(tAppCmdboxDatabaseListUnit)
};
static const tAppCmdboxDatabaseListUnit CMD_D_LIST[] = {
  {
    .keyword  = "DISPBR",
    .callback = app_cmdbox_callback_1args_DISPBR,
    .nargs    = 1
  },
  {
    .keyword  = "DISPOFF",
    .callback = app_cmdbox_callback_0args_DISPOFF,
    .nargs    = 0
  },
  {
    .keyword  = "DISPON",
    .callback = app_cmdbox_callback_0args_DISPON,
    .nargs    = 0
  }
};
static const tAppCmdboxDatabaseList CMD_D = {
  .database = &CMD_D_LIST[0],
  .len      = sizeof(CMD_D_LIST)/sizeof(tAppCmdboxDatabaseListUnit)
};
static const tAppCmdboxDatabaseListUnit CMD_G_LIST[] = {
  {
    .keyword  = "GT",
    .callback = app_cmdbox_callback_0args_GT,
    .nargs    = 0
  }
};
static const tAppCmdboxDatabaseList CMD_G = {
  .database = &CMD_G_LIST[0],
  .len      = sizeof(CMD_G_LIST)/sizeof(tAppCmdboxDatabaseListUnit)
};
static const tAppCmdboxDatabaseListUnit CMD_S_LIST[] = {
  {
    .keyword  = "ST",
    .callback = app_cmdbox_callback_6args_ST,
    .nargs    = 6
  }
};
static const tAppCmdboxDatabaseList CMD_S = {
  .database = &CMD_S_LIST[0],
  .len      = sizeof(CMD_S_LIST)/sizeof(tAppCmdboxDatabaseListUnit)
};

static const tAppCmdboxDatabaseList *CMD_TABLE[] = {
  [('A'-'A') ... ('B'-'A')] = &CMD_DUMMY,
  [('C'-'A')]               = &CMD_C,
  [('D'-'A')]               = &CMD_D,
  [('E'-'A') ... ('F'-'A')] = &CMD_DUMMY,
  [('G'-'A')]               = &CMD_G,
  [('H'-'A') ... ('R'-'A')] = &CMD_DUMMY,
  [('S'-'A')]               = &CMD_S,
  [('T'-'A') ... ('Z'-'A')] = &CMD_DUMMY
};

static char rx_buf_copy[BSP_CFG_UART_RX_BUF_SIZE];


int app_cmdbox_callback_wrapper_0args(const char *cmd, int(*callback)(const char *, ...), int *args) {
  return callback(cmd);
}
int app_cmdbox_callback_wrapper_1args(const char *cmd, int(*callback)(const char *, ...), int *args) {
  return callback(cmd, args[0]);
}
int app_cmdbox_callback_wrapper_2args(const char *cmd, int(*callback)(const char *, ...), int *args) {
  return callback(cmd, args[0], args[1]);
}
int app_cmdbox_callback_wrapper_6args(const char *cmd, int(*callback)(const char *, ...), int *args) {
  return callback(cmd, args[0], args[1], args[2], args[3], args[4], args[5]);
}

int (*app_cmdbox_callback_wrapper[7])(const char *cmd, int(*callback)(const char *, ...), int *args) = {
  app_cmdbox_callback_wrapper_0args,
  app_cmdbox_callback_wrapper_1args,
  app_cmdbox_callback_wrapper_2args,
  NULL,
  NULL,
  NULL,
  app_cmdbox_callback_wrapper_6args
};

/* ************************************************************************** */
/*                             Private Functions                              */
/* ************************************************************************** */

typedef cmnBoolean_t (*tAppCmnBoxStrCmpFunc)(char);


static cmnBoolean_t app_cmdbox_string_is_LF(char c) {
  return (c == '\r');
}

static cmnBoolean_t app_cmdbox_string_is_CR(char c) {
  return (c == '\n');
}

static cmnBoolean_t app_cmdbox_string_is_NULL(char c) {
  return (c == '\0');
}

static cmnBoolean_t app_cmdbox_string_is_enter(char c) {
  return app_cmdbox_string_is_LF(c) || app_cmdbox_string_is_CR(c) || app_cmdbox_string_is_NULL(c);
}

static cmnBoolean_t app_cmdbox_string_is_SP(char c) {
  return (c == '\x20');
}

static cmnBoolean_t app_cmdbox_string_is_digit(char c) {
  return (0x00 != isdigit(c));
}


static size_t app_cmdbox_string_skip_until(const char *cmd, tAppCmnBoxStrCmpFunc comparator, cmnBoolean_t reversed, const char *p_end) {
  size_t cursor = 0;
  while ( ((true == comparator(cmd[cursor])) ^ reversed) && (&cmd[cursor] != p_end)) {
    ++cursor;
  }
  return cursor;
}

static size_t app_cmdbox_parse_search(const char *cmd, const tAppCmdboxDatabaseListUnit **pp_matched_cmd) {
  const tAppCmdboxDatabaseListUnit *p_matched_cmd = NULL;
  size_t                           cursor         = 0;

  if (isalpha((int)cmd[0]) && cmd[0] >= 'A') {
    const tAppCmdboxDatabaseList *cmd_database_list = CMD_TABLE[cmd[cursor]-'A'];
    /* Search if command was supported */
    const char *ptr   = NULL;
    for( int i = 0; i < cmd_database_list->len; ++i){
      ptr = strstr( &cmd[cursor], cmd_database_list->database[i].keyword);
      if (ptr == &cmd[cursor]) {
        p_matched_cmd = &cmd_database_list->database[i];
        TRACE_INFO("Received user command: [%s]", &cmd[cursor]);
        cursor = strlen(cmd_database_list->database[i].keyword);
        break;
      }
    }
  }
  ASSERT( (!p_matched_cmd) || (p_matched_cmd->callback), "Found a command with a null callback" );
  *pp_matched_cmd = p_matched_cmd;
  return cursor;
}

/**
 * @brief Parse the user commands
 * @attention
 *  - `cmd` MUST be a null terminated string
 *  - `cmd` MUST follow this pattern: {<str> [int1] ... <\r|\n>  <\0>}
 * @param [in] cmd - User Command
 */
void app_cmdbox_parse(const char *cmd) {
  if (cmd==NULL) {
    return;
  }

  char        *cmd_copy = rx_buf_copy;
  char * const cmd_end  = stpncpy(cmd_copy, cmd, BSP_CFG_UART_RX_BUF_SIZE);

  ASSERT( (*cmd_end == '\0'), "Command string must be null terminated!!");

  for ( int i = 0; cmd_copy[i] != '\0'; ++i) {
    if (cmd_copy[i] == '\r' || cmd_copy[i] == '\n') {
      cmd_copy[i] = '\0';
    }else {
      cmd_copy[i] = toupper(cmd_copy[i]);
    }
  }

  size_t cursor = 0;

  while (1) {
    /* Skip escaping charactors */
    cursor += app_cmdbox_string_skip_until(&cmd_copy[cursor], app_cmdbox_string_is_enter, false, cmd_end);
    
    if (&cmd_copy[cursor] == cmd_end) {
      break;
    }

    const tAppCmdboxDatabaseListUnit *p_matched_cmd = NULL;

    cursor += app_cmdbox_parse_search(&cmd_copy[cursor], &p_matched_cmd);

    if (NULL == p_matched_cmd) {
      TRACE_WARNING("=> Unknown command: [%s]", &cmd_copy[cursor]);

      /* Skip until the user enter reached */
      cursor += app_cmdbox_string_skip_until(&cmd_copy[cursor], app_cmdbox_string_is_enter, true, cmd_end);
      continue;
    }
    /* Extract arguments */
    ASSERT(p_matched_cmd->nargs <= MAX_NUN_ARGS_SUPPORTED, "Too many arguments.");
    arg_t args[MAX_NUN_ARGS_SUPPORTED];
    for (int i = 0; i < p_matched_cmd->nargs; ++i) {
      
      /* Skip the space */
      cursor += app_cmdbox_string_skip_until(&cmd_copy[cursor], app_cmdbox_string_is_SP, false, cmd_end);

      uint8_t cursor_arg_start = cursor;
      /* Skip digits */
      cursor += app_cmdbox_string_skip_until(&cmd_copy[cursor], app_cmdbox_string_is_digit, false, cmd_end);

      {
        char tmp = cmd_copy[cursor];
        cmd_copy[cursor] = '\0';
        args[i] = atoi( &cmd_copy[cursor_arg_start]);
        cmd_copy[cursor] = tmp;

        /* Execute */
#if (defined SYS_TARGET_NATIVE)
        TRACE_DEBUG("\targ[%d]=%d", i, args[i]);
#endif
      }
    }
    /* Execute */
    app_cmdbox_callback_wrapper[p_matched_cmd->nargs]( p_matched_cmd->keyword, p_matched_cmd->callback, args);
  }

}

/* ************************************************************************** */
/*                        Public Command Box Function                         */
/* ************************************************************************** */
#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || (defined EMULATOR_STM32F405RGT6)
void app_cmdbox_main(void *param) RTOSTHREAD {
  tRtosEvent *p_event = &metope.app.rtos.event;
  tBspUart   *p_uart  = ((tBspUart *)param);

  xEventGroupClearBits(p_event->_handle, CMN_EVENT_UART_INPUT);
  while(1){
    EventBits_t uxBits = xEventGroupWaitBits( p_event->_handle, CMN_EVENT_UART_INPUT, pdFALSE, pdFALSE, portMAX_DELAY);

    if(uxBits & CMN_EVENT_UART_INPUT){
      if (p_uart->rx_status.error_code) {
        TRACE_WARNING("Can NOT parse the command due to a RX error. Data will be flushed: error_code=0x%02X rawstr=%s rd_idx=%d", p_uart->rx_status.error_code, p_uart->rx_buf, p_uart->rx_idx);
        p_uart->rx_status.error_code = 0;
      }
      else if (p_uart->rx_status.has_new_msg || p_uart->rx_status.is_overflowed) {
        /**
         * @bug `is_locked` was NOT safe enough.
         */
        p_uart->rx_status.is_locked     = 1;
        app_cmdbox_parse(p_uart->rx_buf);
      }

      p_uart->rx_idx                  = 0;
      p_uart->rx_status.has_new_msg   = 0;
      p_uart->rx_status.is_overflowed = 0;
      p_uart->rx_status.is_locked     = 0;
      xEventGroupClearBits(p_event->_handle, CMN_EVENT_UART_INPUT);
    }
  }
}
#endif

#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || (defined EMULATOR_STM32F405RGT6)
void app_cmdbox_idle(void *param) RTOSIDLE {

}
#endif

#ifdef __cplusplus
}
#endif
