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

/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include "device.h"
#include "global.h"
#include "trace.h"
#include "bsp_battery.h"


/* ************************************************************************** */
/*                               Private Macros                               */
/* ************************************************************************** */
#define ADC_REPETITION_POW2   3
#define ADC_REPETITION        (1<<ADC_REPETITION_POW2)

#define MIN_BATTERY_ADC_VALUE 1850
#define MAX_BATTERY_ADC_VALUE 2450


/* ************************************************************************** */
/*                              Public Functions                              */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"{
#endif
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

  // TRACE_DEBUG("BSP - Battery ADC raw=%u raw_copy=%u ans=%u", raw, raw_copy, ans);
  return ans;
}


#ifdef __cplusplus
}
#endif



  // DEBUG: BSP - Battery ADC raw=2600 raw_copy=2450 ans=255
  // DEBUG: BSP - Battery ADC raw=2460 raw_copy=2450 ans=255
  // DEBUG: BSP - Battery ADC raw=2382 raw_copy=2382 ans=226
  // DEBUG: BSP - Battery ADC raw=2319 raw_copy=2319 ans=199
  // DEBUG: CMN - Time increased to 2025/1/21 1:40:0
  // DEBUG: BSP - Battery ADC raw=2403 raw_copy=2403 ans=235
  // DEBUG: BSP - Battery ADC raw=2336 raw_copy=2336 ans=207
  // DEBUG: BSP - Battery ADC raw=2241 raw_copy=2241 ans=166
  // DEBUG: BSP - Battery ADC raw=2327 raw_copy=2327 ans=203
  // DEBUG: BSP - Battery ADC raw=2310 raw_copy=2310 ans=195
  // DEBUG: BSP - Battery ADC raw=2279 raw_copy=2279 ans=182
  // DEBUG: BSP - Battery ADC raw=2286 raw_copy=2286 ans=185
  // DEBUG: BSP - Battery ADC raw=2328 raw_copy=2328 ans=203
  // DEBUG: BSP - Battery ADC raw=2287 raw_copy=2287 ans=186
  // DEBUG: BSP - Battery ADC raw=2266 raw_copy=2266 ans=177
  // DEBUG: BSP - Battery ADC raw=2318 raw_copy=2318 ans=199
  // DEBUG: BSP - Battery ADC raw=2282 raw_copy=2282 ans=184
  // DEBUG: BSP - Battery ADC raw=2284 raw_copy=2284 ans=184
  // DEBUG: BSP - Battery ADC raw=2336 raw_copy=2336 ans=207
  // DEBUG: BSP - Battery ADC raw=2262 raw_copy=2262 ans=175
  // DEBUG: BSP - Battery ADC raw=2278 raw_copy=2278 ans=182
  // DEBUG: BSP - Battery ADC raw=2260 raw_copy=2260 ans=174
  // DEBUG: BSP - Battery ADC raw=2271 raw_copy=2271 ans=179
  // DEBUG: BSP - Battery ADC raw=2310 raw_copy=2310 ans=195
  // DEBUG: BSP - Battery ADC raw=2334 raw_copy=2334 ans=206
  // DEBUG: CMN - Time increased to 2025/1/21 1:41:0
  // DEBUG: BSP - Battery ADC raw=2255 raw_copy=2255 ans=172
  // DEBUG: BSP - Battery ADC raw=2299 raw_copy=2299 ans=191
  // DEBUG: BSP - Battery ADC raw=2254 raw_copy=2254 ans=172
  // DEBUG: BSP - Battery ADC raw=2266 raw_copy=2266 ans=177
  // DEBUG: BSP - Battery ADC raw=2260 raw_copy=2260 ans=174
  // DEBUG: BSP - Battery ADC raw=2270 raw_copy=2270 ans=178
  // DEBUG: BSP - Battery ADC raw=2260 raw_copy=2260 ans=174
  // DEBUG: BSP - Battery ADC raw=2347 raw_copy=2347 ans=211
  // DEBUG: BSP - Battery ADC raw=2220 raw_copy=2220 ans=157
  // DEBUG: BSP - Battery ADC raw=2296 raw_copy=2296 ans=189
  // DEBUG: BSP - Battery ADC raw=2254 raw_copy=2254 ans=172
  // DEBUG: BSP - Battery ADC raw=2234 raw_copy=2234 ans=163
  // DEBUG: BSP - Battery ADC raw=2196 raw_copy=2196 ans=147
  // DEBUG: BSP - Battery ADC raw=2251 raw_copy=2251 ans=170
  // DEBUG: BSP - Battery ADC raw=2248 raw_copy=2248 ans=169
  // DEBUG: BSP - Battery ADC raw=2334 raw_copy=2334 ans=206
  // DEBUG: BSP - Battery ADC raw=2294 raw_copy=2294 ans=189
  // DEBUG: BSP - Battery ADC raw=2244 raw_copy=2244 ans=167
  // DEBUG: BSP - Battery ADC raw=2271 raw_copy=2271 ans=179
  // DEBUG: BSP - Battery ADC raw=2296 raw_copy=2296 ans=189
  // DEBUG: CMN - Time increased to 2025/1/21 1:42:0
  // DEBUG: BSP - Battery ADC raw=2366 raw_copy=2366 ans=219
  // DEBUG: BSP - Battery ADC raw=2135 raw_copy=2135 ans=121
  // DEBUG: BSP - Battery ADC raw=2231 raw_copy=2231 ans=162
  // DEBUG: BSP - Battery ADC raw=2094 raw_copy=2094 ans=103
  // DEBUG: BSP - Battery ADC raw=2295 raw_copy=2295 ans=189
  // DEBUG: BSP - Battery ADC raw=2222 raw_copy=2222 ans=158
  // DEBUG: BSP - Battery ADC raw=2286 raw_copy=2286 ans=185
  // DEBUG: BSP - Battery ADC raw=2245 raw_copy=2245 ans=168
  // DEBUG: BSP - Battery ADC raw=2226 raw_copy=2226 ans=160
  // DEBUG: BSP - Battery ADC raw=2242 raw_copy=2242 ans=166
  // DEBUG: BSP - Battery ADC raw=2239 raw_copy=2239 ans=165
  // DEBUG: BSP - Battery ADC raw=2252 raw_copy=2252 ans=171
  // DEBUG: BSP - Battery ADC raw=2226 raw_copy=2226 ans=160
  // DEBUG: BSP - Battery ADC raw=2210 raw_copy=2210 ans=153
  // DEBUG: BSP - Battery ADC raw=2234 raw_copy=2234 ans=163
  // DEBUG: BSP - Battery ADC raw=2239 raw_copy=2239 ans=165
  // DEBUG: BSP - Battery ADC raw=2218 raw_copy=2218 ans=156
  // DEBUG: BSP - Battery ADC raw=2277 raw_copy=2277 ans=181
  // DEBUG: BSP - Battery ADC raw=2234 raw_copy=2234 ans=163
  // DEBUG: BSP - Battery ADC raw=2287 raw_copy=2287 ans=186
  // DEBUG: CMN - Time increased to 2025/1/21 1:43:0
  // DEBUG: BSP - Battery ADC raw=2230 raw_copy=2230 ans=161
  // DEBUG: BSP - Battery ADC raw=2230 raw_copy=2230 ans=161
  // DEBUG: BSP - Battery ADC raw=2240 raw_copy=2240 ans=166
  // DEBUG: BSP - Battery ADC raw=2190 raw_copy=2190 ans=144
  // DEBUG: BSP - Battery ADC raw=2266 raw_copy=2266 ans=177
  // DEBUG: BSP - Battery ADC raw=2223 raw_copy=2223 ans=158
  // DEBUG: BSP - Battery ADC raw=2330 raw_copy=2330 ans=204
  // DEBUG: BSP - Battery ADC raw=2223 raw_copy=2223 ans=158
  // DEBUG: BSP - Battery ADC raw=2231 raw_copy=2231 ans=162
  // DEBUG: BSP - Battery ADC raw=2222 raw_copy=2222 ans=158
  // DEBUG: BSP - Battery ADC raw=2286 raw_copy=2286 ans=185
  // DEBUG: BSP - Battery ADC raw=2272 raw_copy=2272 ans=179
  // DEBUG: BSP - Battery ADC raw=2220 raw_copy=2220 ans=157
  // DEBUG: BSP - Battery ADC raw=2228 raw_copy=2228 ans=161
  // DEBUG: BSP - Battery ADC raw=2220 raw_copy=2220 ans=157
  // DEBUG: BSP - Battery ADC raw=2191 raw_copy=2191 ans=145
  // DEBUG: BSP - Battery ADC raw=2249 raw_copy=2249 ans=169
  // DEBUG: BSP - Battery ADC raw=2258 raw_copy=2258 ans=173
  // DEBUG: BSP - Battery ADC raw=2220 raw_copy=2220 ans=157
  // DEBUG: CMN - Time increased to 2025/1/21 1:44:0
  // DEBUG: BSP - Battery ADC raw=2218 raw_copy=2218 ans=156
  // DEBUG: BSP - Battery ADC raw=2210 raw_copy=2210 ans=153
  // DEBUG: BSP - Battery ADC raw=2264 raw_copy=2264 ans=176
  // DEBUG: BSP - Battery ADC raw=2219 raw_copy=2219 ans=157
  // DEBUG: BSP - Battery ADC raw=2219 raw_copy=2219 ans=157
  // DEBUG: BSP - Battery ADC raw=2183 raw_copy=2183 ans=141
  // DEBUG: BSP - Battery ADC raw=2158 raw_copy=2158 ans=131
  // DEBUG: BSP - Battery ADC raw=2250 raw_copy=2250 ans=170
  // DEBUG: BSP - Battery ADC raw=2212 raw_copy=2212 ans=154
  // DEBUG: BSP - Battery ADC raw=2190 raw_copy=2190 ans=144
  // DEBUG: BSP - Battery ADC raw=2230 raw_copy=2230 ans=161
  // DEBUG: BSP - Battery ADC raw=2210 raw_copy=2210 ans=153
  // DEBUG: BSP - Battery ADC raw=2174 raw_copy=2174 ans=138
  // DEBUG: BSP - Battery ADC raw=2282 raw_copy=2282 ans=184
  // DEBUG: BSP - Battery ADC raw=2202 raw_copy=2202 ans=149
  // DEBUG: BSP - Battery ADC raw=2247 raw_copy=2247 ans=169
  // DEBUG: BSP - Battery ADC raw=2262 raw_copy=2262 ans=175
  // DEBUG: BSP - Battery ADC raw=2189 raw_copy=2189 ans=144
  // DEBUG: BSP - Battery ADC raw=2142 raw_copy=2142 ans=124
  // DEBUG: BSP - Battery ADC raw=2223 raw_copy=2223 ans=158
  // DEBUG: CMN - Time increased to 2025/1/21 1:45:0
  // DEBUG: BSP - Battery ADC raw=2280 raw_copy=2280 ans=183
  // DEBUG: BSP - Battery ADC raw=2214 raw_copy=2214 ans=155
  // DEBUG: BSP - Battery ADC raw=2109 raw_copy=2109 ans=110
  // DEBUG: BSP - Battery ADC raw=2198 raw_copy=2198 ans=148
  // DEBUG: BSP - Battery ADC raw=2330 raw_copy=2330 ans=204
  // DEBUG: BSP - Battery ADC raw=2214 raw_copy=2214 ans=155
  // DEBUG: BSP - Battery ADC raw=2198 raw_copy=2198 ans=148
  // DEBUG: BSP - Battery ADC raw=2244 raw_copy=2244 ans=167
  // DEBUG: BSP - Battery ADC raw=2177 raw_copy=2177 ans=139
  // DEBUG: BSP - Battery ADC raw=2211 raw_copy=2211 ans=153
  // DEBUG: BSP - Battery ADC raw=2190 raw_copy=2190 ans=144
  // DEBUG: BSP - Battery ADC raw=2198 raw_copy=2198 ans=148
  // DEBUG: BSP - Battery ADC raw=2318 raw_copy=2318 ans=199
  // DEBUG: BSP - Battery ADC raw=2135 raw_copy=2135 ans=121
  // DEBUG: BSP - Battery ADC raw=2191 raw_copy=2191 ans=145
  // DEBUG: BSP - Battery ADC raw=2190 raw_copy=2190 ans=144
  // DEBUG: BSP - Battery ADC raw=2186 raw_copy=2186 ans=143
  // DEBUG: BSP - Battery ADC raw=2170 raw_copy=2170 ans=136
  // DEBUG: BSP - Battery ADC raw=2184 raw_copy=2184 ans=142
  // DEBUG: CMN - Time increased to 2025/1/21 1:46:0
  // DEBUG: BSP - Battery ADC raw=2183 raw_copy=2183 ans=141
  // DEBUG: BSP - Battery ADC raw=2182 raw_copy=2182 ans=141
  // DEBUG: BSP - Battery ADC raw=2182 raw_copy=2182 ans=141
  // DEBUG: BSP - Battery ADC raw=2274 raw_copy=2274 ans=180
  // DEBUG: BSP - Battery ADC raw=2092 raw_copy=2092 ans=103
  // DEBUG: BSP - Battery ADC raw=2212 raw_copy=2212 ans=154
  // DEBUG: BSP - Battery ADC raw=2222 raw_copy=2222 ans=158
  // DEBUG: BSP - Battery ADC raw=2176 raw_copy=2176 ans=138
  // DEBUG: BSP - Battery ADC raw=2151 raw_copy=2151 ans=128
  // DEBUG: BSP - Battery ADC raw=2169 raw_copy=2169 ans=135
  // DEBUG: BSP - Battery ADC raw=2148 raw_copy=2148 ans=126
  // DEBUG: BSP - Battery ADC raw=2172 raw_copy=2172 ans=137
  // DEBUG: BSP - Battery ADC raw=2155 raw_copy=2155 ans=129
  // DEBUG: BSP - Battery ADC raw=2150 raw_copy=2150 ans=127
  // DEBUG: BSP - Battery ADC raw=2224 raw_copy=2224 ans=159
  // DEBUG: BSP - Battery ADC raw=2142 raw_copy=2142 ans=124
  // DEBUG: BSP - Battery ADC raw=2146 raw_copy=2146 ans=126
  // DEBUG: BSP - Battery ADC raw=2142 raw_copy=2142 ans=124
  // DEBUG: BSP - Battery ADC raw=2140 raw_copy=2140 ans=123
  // DEBUG: BSP - Battery ADC raw=2166 raw_copy=2166 ans=134
  // DEBUG: CMN - Time increased to 2025/1/21 1:47:0
  // DEBUG: BSP - Battery ADC raw=2127 raw_copy=2127 ans=117
  // DEBUG: BSP - Battery ADC raw=2163 raw_copy=2163 ans=133
  // DEBUG: BSP - Battery ADC raw=2137 raw_copy=2137 ans=122
  // DEBUG: BSP - Battery ADC raw=2111 raw_copy=2111 ans=111
  // DEBUG: BSP - Battery ADC raw=2117 raw_copy=2117 ans=113
  // DEBUG: BSP - Battery ADC raw=2114 raw_copy=2114 ans=112
  // DEBUG: BSP - Battery ADC raw=2150 raw_copy=2150 ans=127
  // DEBUG: BSP - Battery ADC raw=2152 raw_copy=2152 ans=128
  // DEBUG: BSP - Battery ADC raw=2103 raw_copy=2103 ans=107
  // DEBUG: BSP - Battery ADC raw=2096 raw_copy=2096 ans=104
  // DEBUG: BSP - Battery ADC raw=2093 raw_copy=2093 ans=103
  // DEBUG: BSP - Battery ADC raw=2053 raw_copy=2053 ans=86
  // DEBUG: BSP - Battery ADC raw=2106 raw_copy=2106 ans=109
  // DEBUG: BSP - Battery ADC raw=2102 raw_copy=2102 ans=107
  // DEBUG: BSP - Battery ADC raw=2062 raw_copy=2062 ans=90
  // DEBUG: BSP - Battery ADC raw=2070 raw_copy=2070 ans=93
  // DEBUG: BSP - Battery ADC raw=2105 raw_copy=2105 ans=108
  // DEBUG: BSP - Battery ADC raw=2059 raw_copy=2059 ans=89
  // DEBUG: BSP - Battery ADC raw=2063 raw_copy=2063 ans=90
  // DEBUG: BSP - Battery ADC raw=2054 raw_copy=2054 ans=86
  // DEBUG: CMN - Time increased to 2025/1/21 1:48:0
  // DEBUG: BSP - Battery ADC raw=2055 raw_copy=2055 ans=87
  // DEBUG: BSP - Battery ADC raw=2143 raw_copy=2143 ans=124
  // DEBUG: BSP - Battery ADC raw=2046 raw_copy=2046 ans=83
  // DEBUG: BSP - Battery ADC raw=2064 raw_copy=2064 ans=91
  // DEBUG: BSP - Battery ADC raw=2062 raw_copy=2062 ans=90
  // DEBUG: BSP - Battery ADC raw=2054 raw_copy=2054 ans=86
  // DEBUG: BSP - Battery ADC raw=2062 raw_copy=2062 ans=90
  // DEBUG: BSP - Battery ADC raw=2131 raw_copy=2131 ans=119
  // DEBUG: BSP - Battery ADC raw=2055 raw_copy=2055 ans=87
  // DEBUG: BSP - Battery ADC raw=2056 raw_copy=2056 ans=87
  // DEBUG: BSP - Battery ADC raw=2047 raw_copy=2047 ans=83
  // DEBUG: BSP - Battery ADC raw=2058 raw_copy=2058 ans=88
  // DEBUG: BSP - Battery ADC raw=2156 raw_copy=2156 ans=130
  // DEBUG: BSP - Battery ADC raw=2008 raw_copy=2008 ans=67
  // DEBUG: BSP - Battery ADC raw=2062 raw_copy=2062 ans=90
  // DEBUG: BSP - Battery ADC raw=2054 raw_copy=2054 ans=86
  // DEBUG: BSP - Battery ADC raw=1986 raw_copy=1986 ans=57
  // DEBUG: BSP - Battery ADC raw=2060 raw_copy=2060 ans=89
  // DEBUG: BSP - Battery ADC raw=2058 raw_copy=2058 ans=88
  // DEBUG: BSP - Battery ADC raw=2062 raw_copy=2062 ans=90
  // DEBUG: CMN - Time increased to 2025/1/21 1:49:0
  // DEBUG: BSP - Battery ADC raw=2035 raw_copy=2035 ans=78
  // DEBUG: BSP - Battery ADC raw=2060 raw_copy=2060 ans=89
  // DEBUG: BSP - Battery ADC raw=2055 raw_copy=2055 ans=87
  // DEBUG: BSP - Battery ADC raw=2078 raw_copy=2078 ans=97
  // DEBUG: BSP - Battery ADC raw=2046 raw_copy=2046 ans=83
  // DEBUG: BSP - Battery ADC raw=2086 raw_copy=2086 ans=100
  // DEBUG: BSP - Battery ADC raw=2062 raw_copy=2062 ans=90
  // DEBUG: BSP - Battery ADC raw=2142 raw_copy=2142 ans=124
  // DEBUG: BSP - Battery ADC raw=2084 raw_copy=2084 ans=99
  // DEBUG: BSP - Battery ADC raw=2062 raw_copy=2062 ans=90
  // DEBUG: BSP - Battery ADC raw=1955 raw_copy=1955 ans=44
  // DEBUG: BSP - Battery ADC raw=2094 raw_copy=2094 ans=103
  // DEBUG: BSP - Battery ADC raw=2059 raw_copy=2059 ans=89
  // DEBUG: BSP - Battery ADC raw=2054 raw_copy=2054 ans=86
  // DEBUG: BSP - Battery ADC raw=2058 raw_copy=2058 ans=88
  // DEBUG: BSP - Battery ADC raw=2054 raw_copy=2054 ans=86
  // DEBUG: BSP - Battery ADC raw=2060 raw_copy=2060 ans=89
  // DEBUG: BSP - Battery ADC raw=2039 raw_copy=2039 ans=80
  // DEBUG: BSP - Battery ADC raw=2066 raw_copy=2066 ans=92
  // DEBUG: CMN - Time increased to 2025/1/21 1:50:0
  // DEBUG: BSP - Battery ADC raw=2055 raw_copy=2055 ans=87
  // DEBUG: BSP - Battery ADC raw=2060 raw_copy=2060 ans=89
  // DEBUG: BSP - Battery ADC raw=2054 raw_copy=2054 ans=86
  // DEBUG: BSP - Battery ADC raw=2038 raw_copy=2038 ans=80
  // DEBUG: BSP - Battery ADC raw=2052 raw_copy=2052 ans=86
  // DEBUG: BSP - Battery ADC raw=2047 raw_copy=2047 ans=83
  // DEBUG: BSP - Battery ADC raw=2046 raw_copy=2046 ans=83
  // DEBUG: BSP - Battery ADC raw=2087 raw_copy=2087 ans=100
  // DEBUG: BSP - Battery ADC raw=2054 raw_copy=2054 ans=86
  // DEBUG: BSP - Battery ADC raw=2063 raw_copy=2063 ans=90
  // DEBUG: BSP - Battery ADC raw=2060 raw_copy=2060 ans=89
  // DEBUG: CMN - Time increased to 2025/1/21 1:51:0
  // DEBUG: BSP - Battery ADC raw=2054 raw_copy=2054 ans=86
  // DEBUG: BSP - Battery ADC raw=2055 raw_copy=2055 ans=87
  // DEBUG: BSP - Battery ADC raw=2060 raw_copy=2060 ans=89
  // DEBUG: BSP - Battery ADC raw=2070 raw_copy=2070 ans=93
  // DEBUG: BSP - Battery ADC raw=2062 raw_copy=2062 ans=90
  // DEBUG: BSP - Battery ADC raw=2061 raw_copy=2061 ans=89
  // DEBUG: BSP - Battery ADC raw=2060 raw_copy=2060 ans=89
  // DEBUG: BSP - Battery ADC raw=2052 raw_copy=2052 ans=86
  // DEBUG: BSP - Battery ADC raw=1974 raw_copy=1974 ans=52
  // DEBUG: BSP - Battery ADC raw=2062 raw_copy=2062 ans=90
  // DEBUG: BSP - Battery ADC raw=2054 raw_copy=2054 ans=86
  // DEBUG: BSP - Battery ADC raw=2063 raw_copy=2063 ans=90
  // DEBUG: BSP - Battery ADC raw=2062 raw_copy=2062 ans=90
  // DEBUG: BSP - Battery ADC raw=2188 raw_copy=2188 ans=143
  // DEBUG: BSP - Battery ADC raw=2047 raw_copy=2047 ans=83
  // DEBUG: BSP - Battery ADC raw=2047 raw_copy=2047 ans=83
  //  INFO: Initialization Completed
  // DEBUG: BSP - Battery ADC raw=2614 raw_copy=2450 ans=255
  // DEBUG: BSP - Battery ADC raw=2558 raw_copy=2450 ans=255
  // DEBUG: BSP - Battery ADC raw=2542 raw_copy=2450 ans=255


/* ********************************** EOF *********************************** */
