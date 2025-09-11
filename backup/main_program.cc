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
  app_init();
  os_init();

  TRACE_INFO("System boot completed.");

#if 1
  {
    const volatile uint32_t REG_SCB_CPUID = SCB->CPUID;
    TRACE_DEBUG("Processor Manufacturer Info:");
    TRACE_DEBUG("+------------------------------------------------------+");
    TRACE_DEBUG("| Implementer | Variant | Constant | PartNo | Revision |");
    TRACE_DEBUG("|-------------+---------+----------+--------+----------|");
    TRACE_DEBUG("| 0x%02X        | 0x%02X    | 0x%01X      | 0x%03X  | 0x%01X      |",\
      ((REG_SCB_CPUID & SCB_CPUID_IMPLEMENTER_Msk ) >> SCB_CPUID_IMPLEMENTER_Pos ),\
      ((REG_SCB_CPUID & SCB_CPUID_VARIANT_Msk     ) >> SCB_CPUID_VARIANT_Pos     ),\
      ((REG_SCB_CPUID & SCB_CPUID_ARCHITECTURE_Msk) >> SCB_CPUID_ARCHITECTURE_Pos),\
      ((REG_SCB_CPUID & SCB_CPUID_PARTNO_Msk      ) >> SCB_CPUID_PARTNO_Pos      ),\
      ((REG_SCB_CPUID & SCB_CPUID_REVISION_Msk    ) >> SCB_CPUID_REVISION_Pos    ) \
    );
    TRACE_DEBUG("+------------------------------------------------------+");
    TRACE_DEBUG("\n");
    UNUSED(REG_SCB_CPUID);
  }
#endif

#if 1
  {
    const volatile uint32_t REG_SCB_AIRCR = SCB->AIRCR;
    TRACE_DEBUG("Application Interrupt Reset Control Register:");
    TRACE_DEBUG("\tis_little_endian         %d", ((REG_SCB_AIRCR & SCB_AIRCR_ENDIANESS_Msk) >> SCB_AIRCR_ENDIANESS_Pos));
    TRACE_DEBUG("\tinterrupt_priority_group %d", ((REG_SCB_AIRCR & SCB_AIRCR_PRIGROUP_Msk) >> SCB_AIRCR_PRIGROUP_Pos));
    TRACE_DEBUG("\n");
  }
#endif

#if 1
  {
    const volatile uint32_t *REG_NVIC_ISER = NVIC->ISER;
    TRACE_DEBUG("Nested Vectored Interrupt Controller:");
    TRACE_DEBUG("\tis_irq_enabled   %d", (0 == __get_PRIMASK()));
    TRACE_DEBUG("\tUSART1_IRQn      %d", (0 != (REG_NVIC_ISER[USART1_IRQn     >> 5] & (1 << (USART1_IRQn     & 0x1F)))));
    TRACE_DEBUG("\tWWDG_IRQn        %d", (0 != (REG_NVIC_ISER[WWDG_IRQn       >> 5] & (1 << (WWDG_IRQn       & 0x1F)))));    /*!< Window WatchDog Interrupt                                         */
    TRACE_DEBUG("\tPVD_IRQn         %d", (0 != (REG_NVIC_ISER[PVD_IRQn        >> 5] & (1 << (PVD_IRQn        & 0x1F)))));    /*!< PVD through EXTI Line detection Interrupt                         */
    TRACE_DEBUG("\tTAMP_STAMP_IRQn  %d", (0 != (REG_NVIC_ISER[TAMP_STAMP_IRQn >> 5] & (1 << (TAMP_STAMP_IRQn & 0x1F)))));    /*!< Tamper and TimeStamp interrupts through the EXTI line             */
    TRACE_DEBUG("\tRTC_WKUP_IRQn    %d", (0 != (REG_NVIC_ISER[RTC_WKUP_IRQn   >> 5] & (1 << (RTC_WKUP_IRQn   & 0x1F)))));    /*!< RTC Wakeup interrupt through the EXTI line                        */
    TRACE_DEBUG("\tFLASH_IRQn       %d", (0 != (REG_NVIC_ISER[FLASH_IRQn      >> 5] & (1 << (FLASH_IRQn      & 0x1F)))));    /*!< FLASH global Interrupt                                            */
    TRACE_DEBUG("\tRCC_IRQn         %d", (0 != (REG_NVIC_ISER[RCC_IRQn        >> 5] & (1 << (RCC_IRQn        & 0x1F)))));    /*!< RCC global Interrupt                                              */
    TRACE_DEBUG("\tUSART2_IRQn      %d", (0 != (REG_NVIC_ISER[USART2_IRQn     >> 5] & (1 << (USART2_IRQn     & 0x1F)))));
    TRACE_DEBUG("\tEXTI15_10_IRQn   %d", (0 != (REG_NVIC_ISER[EXTI15_10_IRQn  >> 5] & (1 << (EXTI15_10_IRQn  & 0x1F)))));
    TRACE_DEBUG("\tTIM2_IRQn        %d", (0 != (REG_NVIC_ISER[TIM2_IRQn       >> 5] & (1 << (TIM2_IRQn       & 0x1F)))));
    TRACE_DEBUG("\n");
  }
#endif

#if 1
  {
    const volatile uint32_t REG_RCC_PLLCFGR = RCC->PLLCFGR;
    const volatile uint32_t REG_RCC_CFGR    = RCC->CFGR;
    
    uint32_t PLL_SRC = ((REG_RCC_PLLCFGR & RCC_PLLCFGR_PLLSRC_Msk) >> RCC_PLLCFGR_PLLSRC_Pos);
    uint32_t f_hse = 25000000;  
    uint32_t f_hsi = 16000000; 
    uint32_t PLL_I = PLL_SRC == 0 ? f_hsi : f_hse;
    uint32_t PLL_N = ((REG_RCC_PLLCFGR & RCC_PLLCFGR_PLLN_Msk) >> RCC_PLLCFGR_PLLN_Pos);
    uint32_t PLL_M = ((REG_RCC_PLLCFGR & RCC_PLLCFGR_PLLM_Msk) >> RCC_PLLCFGR_PLLM_Pos);
    uint32_t PLL_P = ((REG_RCC_PLLCFGR & RCC_PLLCFGR_PLLP_Msk) >> RCC_PLLCFGR_PLLP_Pos);
    uint32_t f_pll = (PLL_I / 1000 * PLL_N / PLL_M) / ((PLL_P+1)*2) * 1000;

    uint32_t clk_src = ((REG_RCC_CFGR & RCC_CFGR_SWS_Msk) >> RCC_CFGR_SWS_Pos);
    const char     *MSG_CLK_SRC[4]  = { "HSI", "HSE", "PLL", "N/A" };
    const uint32_t  MSG_CLK_FREQ[4] = { f_hsi, f_hse, f_pll, 0     };

    uint32_t f_sys = MSG_CLK_FREQ[clk_src];
    uint32_t hpre = ((REG_RCC_CFGR & RCC_CFGR_HPRE_Msk) >> RCC_CFGR_HPRE_Pos);
    if (hpre < 0b1000) {
      hpre = 0b0111;
    }
    uint32_t f_ahb = f_sys >> (hpre + 1 - 0b1000);

    uint32_t ppre2 = ((REG_RCC_PLLCFGR & RCC_CFGR_PPRE2_Msk) >> RCC_CFGR_PPRE2_Pos);
    if (ppre2 < 0b100) {
      ppre2 = 0b011;
    }
    uint32_t f_apb2 = f_ahb >> (ppre2 + 1 - 0b100);

    uint32_t ppre1 = ((REG_RCC_PLLCFGR & RCC_CFGR_PPRE1_Msk) >> RCC_CFGR_PPRE1_Pos);
    if (ppre1 < 0b100) {
      ppre1 = 0b011;
    }
    uint32_t f_apb1 = f_ahb >> (ppre1 + 1 - 0b100);


    TRACE_DEBUG("System Clock Info:");
    TRACE_DEBUG("\tHSI                 %d MHz", f_hsi / 1000000);
    TRACE_DEBUG("\tHSE                 %d MHz", f_hse / 1000000);
    TRACE_DEBUG("\tPLL                 %d MHz", f_pll / 1000000);
    TRACE_DEBUG("\tsystem clk source   [%s]", MSG_CLK_SRC[clk_src]);
    TRACE_DEBUG("\tsystem clk freq     %d MHz", f_sys / 1000000);
    TRACE_DEBUG("\tAHB clk freq        %d MHz", f_ahb / 1000000);
    TRACE_DEBUG("\tAPB1 clk freq       %d MHz", f_apb1 / 1000000);
    TRACE_DEBUG("\tAPB2 clk freq       %d MHz", f_apb2 / 1000000);
    TRACE_DEBUG("\n");
  }
#endif

  app_rtos_start();

  while(1);
  {
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

#if (defined INCLUDE_TB_CMN) && (INCLUDE_TB_CMN==1)
  add_cmn_test();
  cout<<"Local Project Test:"<<endl;
  tb_infra_local.verdict();
#endif

#if (defined INCLUDE_TB_BSP) && (INCLUDE_TB_BSP==1)
  add_bsp_test();
  cout<<"BSP Test:"<<endl;
  tb_infra_bsp.verdict();
#endif
  
#if (defined INCLUDE_TB_OS) && (INCLUDE_TB_OS==1)
  add_app_test();
  cout<<"RTOS Test:"<<endl;
  tb_infra_os.verdict();
#endif
  
  while(1){}
}

#endif
