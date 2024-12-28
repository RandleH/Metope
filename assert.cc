/**
 ******************************************************************************
 * @file    assert.cc
 * @author  RandleH
 * @brief   Project Assertion
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


#include <errno.h>
#include "device.h"
#include "bsp_led.h"

#ifdef DEBUG
void ASSERT( bool expr, const char *msg){
  if(!expr){
    bsp_led_on();
    __BKPT(0);
  }
}



#endif