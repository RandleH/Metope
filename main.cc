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
  
  bsp_led__switch(ON);
  
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
#include "global.h"



LocalProjectTest     tb_infra_local(std::cout);
HumanInteractionTest tb_infra_hmi(std::cout);


int main(int argc, char *argv[]){
  hw_init();
  bsp_init();
  
  std::cout<<"Hardware Initialization Completed.\n\n"<<std::endl;
  
  add_cmn_test();
  add_bsp_test();

  std::cout<<"Local Project Test:"<<std::endl;
  tb_infra_local.verdict();

  std::cout<<"Human Interaction Test:"<<std::endl;
  tb_infra_hmi.verdict();

  while(1){}
}

#endif
