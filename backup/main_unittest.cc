/**
 ******************************************************************************
 * @file    main_unittest.cc
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
#include "cmn_delay.h"
#include "test.hh"
#include "cmn_test.hh"
#include "bsp_test.hh"
#include "app_test.hh"
#include "bsp_cpu.h"

/* ************************************************************************** */
/*                             Compiling Assertion                            */
/* ************************************************************************** */
#if !(defined UNIT_TEST) || (UNIT_TEST==0)
  #error "Project configuration error."
#endif

#if !(defined __cplusplus)
  #error "Unit test should run in c++."
#endif

/* ************************************************************************** */
/*                                    Main                                    */
/* ************************************************************************** */
int main(int argc, char *argv[]){
  hw_init();
  bsp_init();
  // app_init();
  os_init();
  cout<<"System boot completed.\n\n"<<endl;
  
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
