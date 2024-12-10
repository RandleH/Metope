/**
 ******************************************************************************
 * @file    main.c
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


#include "init.h"
#include "cmn_type.h"
#include "bsp_led.h"

#include "app_rtos.h"
#include "global.h"



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

  app_rtos_start();
  
  while(1){

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
