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
#include "app_cmdbox.h"
#include "global.h"
#include "assert.h"
#include "trace.h"
#include "FreeRTOS.h"

/* ************************************************************************** */
/*                        Public Command Box Function                         */
/* ************************************************************************** */
void app_cmdbox_main(void *param) RTOSTHREAD {
  tRtosEvent *p_event = &metope.app.rtos.event;

  xEventGroupClearBits(p_event->_handle, CMN_EVENT_UART_INPUT);
  while(1){
    EventBits_t uxBits = xEventGroupWaitBits( p_event->_handle, CMN_EVENT_UART_INPUT, pdFALSE, pdFALSE, portMAX_DELAY);

    if(uxBits & CMN_EVENT_UART_INPUT){
      TRACE_INFO("Received UART msg: ");
      //...//

      xEventGroupClearBits(p_event->_handle, CMN_EVENT_UART_INPUT);
    }
  }
}

void app_cmdbox_idle(void *param) RTOSIDLE {

}