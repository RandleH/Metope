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


int main(int argc, char *argv[]){
  
  /**
   * @note
   *  Hardware Initialization
   *  @cpu: CortexM4 STM32F411CEU6 96MHz
   *  @periphral: AHB=96MHz APB1=48MHz APB2=96MHz USB=48MHz HSE=25MHz LSE=32.768KHz
   * 
   *  @cpu: CortexM4 STM32F405RGT6 96MHz
   *  @periphral: 
   * 
  */
  hw_init();

  while(1){
      
  }
}