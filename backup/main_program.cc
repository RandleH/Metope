/**
 ******************************************************************************
 * @file    main.cc
 * @author  RandleH
 * @brief   Main program entrance
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
#include "init.h"
#include "global.h"
#include "assert.h"
#include "trace.h"
#include "cmn_type.h"
#include "bsp_led.h"
#include "app_rtos.h"

#include "cmn_delay.h"
#ifndef TEST_ONLY

#ifdef __cplusplus
extern "C"{
#endif

int main(int argc, char *argv[]){
  
  /**
   * @note
   *  Hardware Initialization
   *  @cpu: CortexM4 STM32F411CEU6 96MHz
   *  @timer: SYSTICK=84MHz APB1=96MHz APB2=96MHz 
   *  @periphral: AHB=96MHz APB1=48MHz APB2=96MHz USB=48MHz HSE=25MHz LSE=32.768KHz
   * 
   *  @cpu: CortexM4 STM32F405RGT6 84MHz
   *  @timer: SYSTICK=84MHz APB1=84MHz APB2=84MHz 
   *  @periphral: AHB=84MHz APB1=42MHz APB2=84MHz HSE=25MHz LSE=32.768KHz
   * 
  */
  hw_init();
  bsp_init();
  // app_init();
  // os_init();


  TRACE_INFO("Initialization Completed.");

#if 1
  {
    const volatile uint32_t REG_SCB_CPUID = SCB->CPUID;
    TRACE_INFO("HW_SCB_CPUID_INFO: 0x%08X", REG_SCB_CPUID);
  }
#endif

#if 1
  {
    const volatile uint32_t REG_SCB_AIRCR = SCB->AIRCR;
    uint32_t is_little_endian = (REG_SCB_AIRCR & SCB_AIRCR_ENDIANESS_Msk) >> SCB_AIRCR_ENDIANESS_Pos;
    uint32_t priority_grp     = (REG_SCB_AIRCR & SCB_AIRCR_PRIGROUP_Msk) >> SCB_AIRCR_PRIGROUP_Pos;
    TRACE_INFO("HW_SCB_AIRCR_INFO: is_little_endian=%d priority_grp=%d", is_little_endian, priority_grp);
  }
#endif

#if 1
  {
    const volatile uint32_t *REG_NVIC_ISER = NVIC->ISER;
    uint32_t is_irq_enabled           = (0 == __get_PRIMASK());
    uint32_t is_USART2_IRQn_active    = (0 != (REG_NVIC_ISER[USART2_IRQn >> 5] & (1 << (USART2_IRQn & 0x1F))));
    uint32_t is_EXTI15_10_IRQn_active = (0 != (REG_NVIC_ISER[EXTI15_10_IRQn >> 5] & (1 << (EXTI15_10_IRQn & 0x1F))));
    uint32_t is_TIM2_IRQn_active      = (0 != (REG_NVIC_ISER[TIM2_IRQn >> 5] & (1 << (TIM2_IRQn & 0x1F))));

    TRACE_INFO("HW_NVIC_ISER_INFO: is_irq_enabled=%d\n USART2_IRQn=%d\n EXTI15_10_IRQn=%d\n TIM2_IRQn=%d\n", is_irq_enabled, is_USART2_IRQn_active, is_EXTI15_10_IRQn_active, is_TIM2_IRQn_active);

    // NVIC->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
#endif
  
  // app_rtos_start();

  while(1){
    tBspUart *p_uart = &metope.bsp.uart;
    cmn_timer_delay(1000);
    
    if (p_uart->rx_status.has_new_msg || p_uart->rx_status.is_overflowed) {
      uint8_t cnt = 0;
      char   *ptr = &metope.bsp.uart.rx_buf[cnt];

      while ( *ptr != '\0') {
        while( *ptr != '\n' && *ptr != '\r' && *ptr != '\0') {
          TRACE_INFO("rx_buf[%d]='%c'", cnt++, *ptr++);
        }
        if (*ptr == '\0') {
          break;
        }else {
          TRACE_DEBUG("rx_buf[%d]=%d", cnt++, *ptr++);
        }
      }
      p_uart->rx_idx                  = 0;
      p_uart->rx_status.has_new_msg   = 0;
      p_uart->rx_status.is_overflowed = 0;

      if (p_uart->rx_status.error_code) {
        TRACE_WARNING("rx_error_code=0x%08X", p_uart->rx_status.error_code);
        p_uart->rx_status.error_code = 0;
      }
    }
  }

}

#ifdef __cplusplus
}
#endif

#else

#include "cmn_delay.h"

#include "test.hh"
#include "cmn_test.hh"
#include "bsp_test.hh"
#include "app_test.hh"
#include "bsp_cpu.h"

int main(int argc, char *argv[]){
  hw_init();
  bsp_init();
  // app_init();
  os_init();
  cout<<"Hardware Initialization Completed.\n\n"<<endl;
  
  {
    size_t len = 0;
    auto  *uuid = bsp_cpu_chip_uuid(&len);
    std::cout << "Device Unique Serial ID: ";
    for(size_t i=0; i<len; ++i){
      std::cout << std::setfill('0')<<std::setw(2)<<(int)uuid[i]<< ((i==len-1)?'\n':'-');
    }
    std::cout << "\n\n";
  }

  add_cmn_test();
  add_bsp_test();
  // add_app_test();

#if (defined INCLUDE_TB_CMN) && (INCLUDE_TB_CMN==1)
  cout<<"Local Project Test:"<<endl;
  tb_infra_local.verdict();
#endif

#if (defined INCLUDE_TB_BSP) && (INCLUDE_TB_BSP==1)
  cout<<"BSP Test:"<<endl;
  tb_infra_bsp.verdict();
#endif
  
#if (defined INCLUDE_TB_OS) && (INCLUDE_TB_OS==1)
  cout<<"RTOS Test:"<<endl;
  tb_infra_os.verdict();
#endif
  
  while(1){}
}

#endif
