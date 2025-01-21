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

#define ADC_REPETITION_POW2   3
#define ADC_REPETITION        (1<<ADC_REPETITION_POW2)

#define MIN_BATTERY_ADC_VALUE 1850
#define MAX_BATTERY_ADC_VALUE 2450

/**
 * @note
 *  Analog Battery Voltage Range: 1800mv ~ 2150mv => 1450 ~ 1732 (This value is subject to the actual measurement)
 *  Periphral ADC Range Mapping: 0mv ~ 3300mv => 0 ~ 4095 (12Bit)
 *  Function Output Range: 0 ~ 255 (8Bit)
 */
uint8_t bsp_battery_measure(void){
  uint32_t raw = 0;
  {
    HAL_ADC_Start(metope.dev.pHadc1);

  /**
   * @todo
   *  `HAL_ADC_PollForConversion()` is getting stuck.
   */
#if 0
    for(int i=0; i<ADC_REPETITION; ++i){
      HAL_ADC_PollForConversion(metope.dev.pHadc1, HAL_MAX_DELAY);
      raw += HAL_ADC_GetValue(metope.dev.pHadc1);
    }
    raw >>= ADC_REPETITION_POW2;
#else
    HAL_ADC_PollForConversion(metope.dev.pHadc1, HAL_MAX_DELAY);
    raw = HAL_ADC_GetValue(metope.dev.pHadc1);
#endif

    HAL_ADC_Stop(metope.dev.pHadc1);
  }

  uint32_t raw_copy = raw;

  if( raw_copy > MAX_BATTERY_ADC_VALUE ){
    raw_copy = MAX_BATTERY_ADC_VALUE;
  }else if( raw_copy < MIN_BATTERY_ADC_VALUE ){
    raw_copy = MIN_BATTERY_ADC_VALUE;
  }

  uint8_t ans = (uint8_t)(((raw_copy - MIN_BATTERY_ADC_VALUE)<<8) / (MAX_BATTERY_ADC_VALUE-MIN_BATTERY_ADC_VALUE+1));

  TRACE_DEBUG("BSP - Battery ADC raw=%u raw_copy=%u ans=%u", raw, raw_copy, ans);
  return ans;
}


#ifdef __cplusplus
}
#endif


/* ********************************** EOF *********************************** */
