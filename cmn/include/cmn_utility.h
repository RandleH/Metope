/**
 ******************************************************************************
 * @file    cmn_utility.h
 * @author  RandleH
 * @brief   Common Program - Utility Macros
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
#include "cmn_type.h"


#ifndef CMN_UTILITY_H
#define CMN_UTILITY_H



#define CMN_SWAP( a, b)   do{ (a)^=(b); (b)^=(a); (a)^=(b); }while(0)

#define CMN_MIN( a, b)    ((a)>(b) ? (b) : (a))
#define CMN_MAX( a, b)    ((a)<(b) ? (b) : (a))

#ifdef __cplusplus
extern "C"{
#endif

int32_t cmn_utility_str2dec(const char *str, cmnBoolean_t *isValid);
int32_t cmn_utility_str2hex(const char *str, cmnBoolean_t *isValid);
float cmn_utility_str2float(const char *str, cmnBoolean_t *isValid);

uint8_t cmn_utility_uint2strdec_width( char *str, uint8_t maxlen, uint32_t value, uint8_t width);
uint8_t cmn_utility_uint2strhex_width( char *str, uint8_t maxlen, uint32_t value, uint8_t width);
uint8_t cmn_utility_int2strdec_width( char *str, uint8_t maxlen, int32_t value, uint8_t width);
uint8_t cmn_utility_uint2strdec( char *str, uint8_t maxlen, uint32_t value);
uint8_t cmn_utility_uint2strhex( char *str, uint8_t maxlen, uint32_t value);
uint8_t cmn_utility_int2strdec( char *str, uint8_t maxlen, int32_t value);
uint8_t cmn_utility_int2strhex( char *str, uint8_t maxlen, int32_t value);


char *cmn_utility_strrev(char *str);
char *cmn_utility_strnrev(char *str, size_t len);

void cmn_utility_angleinc( 
                          uint16_t          *hour_rem  ,\
                          uint16_t          *minute_rem,\
                          uint16_t NULLABLE *second_rem,\
                          uint16_t          *hour_inc  ,\
                          uint16_t          *minute_inc,\
                          uint16_t NULLABLE *second_inc,\
                          uint32_t           ms\
                          );
void cmn_utility_angleset( 
                          uint16_t            *hour_rem  ,\
                          uint16_t            *minute_rem,\
                          uint16_t   NULLABLE *second_rem,\
                          uint16_t            *hour_deg  ,\
                          uint16_t            *minute_deg,\
                          uint16_t   NULLABLE *second_deg,\
                          const cmnDateTime_t *pTime
                          );


/**
 * @brief Calculate the microseconds with respect to the second needle angle
 * @param [in] deg - Input angle in the scale of `[0:3599]`
 * @return Return microseconds
 */
static inline uint32_t cmn_utility_secdeg2_ms( uint16_t deg){
#if 0
  return deg * 60 * 1000 / 3600;
#else
  return deg * 50 / 3;
#endif
}

static inline uint32_t cmn_utility_mindeg2_ms( uint16_t deg){
#if 0
  return deg * 60 * 1000 * 60 / 3600;
#else
  return deg * 1000;
#endif
}

static inline uint32_t cmn_utility_hrdeg2_ms( uint16_t deg){
#if 0
  return deg * 60 * 1000 * 60 * 12 / 3600;
#else
  return deg * 12000;
#endif
}


void cmn_utility_timeinc( cmnDateTime_t *pTime, uint32_t ms);

cmnDateTime_t cmn_utility_set_time_from_iso( const char* __timestamp__);

#ifdef __cplusplus
}
#endif

#endif