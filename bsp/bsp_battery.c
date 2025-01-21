/**
 ******************************************************************************
 * @file    bsp_battery.c
 * @author  RandleH
 * @brief   Board Support Package Delivery - Battery
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 RandleH.
 * All rights reserved.
 *
 * This software component is licensed by RandleH under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
*/

#include "device.h"
#include "global.h"
#include "trace.h"
#include "bsp_battery.h"

#ifdef __cplusplus
extern "C"{
#endif


#define MIN_BATTERY_ADC_VALUE 1450
#define MAX_BATTERY_ADC_VALUE 1732

/**
 * @note
 *  Analog Battery Voltage Range: 1800mv ~ 2150mv => 1450 ~ 1732
 *  Periphral ADC Range Mapping: 0mv ~ 3300mv => 0 ~ 4095 (12Bit)
 *  Function Output Range: 0 ~ 255 (8Bit)
 */
uint8_t bsp_battery_measure(void){
  HAL_ADC_Start(metope.dev.pHadc1);
  HAL_ADC_PollForConversion(metope.dev.pHadc1, HAL_MAX_DELAY);
  uint32_t raw = HAL_ADC_GetValue(metope.dev.pHadc1);

  if( raw > MAX_BATTERY_ADC_VALUE ){
    raw = MAX_BATTERY_ADC_VALUE;
  }else if( raw < MIN_BATTERY_ADC_VALUE ){
    raw = MIN_BATTERY_ADC_VALUE;
  }

  uint8_t ans = (uint8_t)(((raw - MIN_BATTERY_ADC_VALUE)<<8) / (MAX_BATTERY_ADC_VALUE-MIN_BATTERY_ADC_VALUE));


  TRACE_DEBUG("BSP - Battery ADC raw=%u ans=%u", raw, ans);
  return ans;
}


#ifdef __cplusplus
}
#endif


/* ********************************** EOF *********************************** */
