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
#include "global.h"


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  bsp_led__toggle();
}


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
  
  bsp_led__switch(ON);
  metope.app.dummy = 0;
  while(1){

    
  }
}

#ifdef __cplusplus
}
#endif

#else


#include <iostream>
#include "test.hh"
#include "cmn_test.hh"
#include "bsp_test.hh"



int main(int argc, char *argv[]){
  hw_init();
  bsp_init();
  app_init();
  // printf( "Device Serial Number: %02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d-%02d\r\n", uid.raw[0],uid.raw[1],uid.raw[2],uid.raw[3],uid.raw[4],uid.raw[5],uid.raw[6],uid.raw[7],uid.raw[8],uid.raw[9],uid.raw[10],uid.raw[11]);
  std::cout<<"Hardware Initialization Completed.\n\n"<<std::endl;
  
  add_cmn_test();
  add_bsp_test();

  std::cout<<"Local Project Test:"<<std::endl;
  tb_infra_local.verdict();

  std::cout<<"Human Interaction Test:"<<std::endl;
  tb_infra_hmi.verdict();
  metope.app.dummy = 0;
  while(1){}
}

#endif
