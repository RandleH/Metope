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

void cmn_utility_int2strhex( char *str, uint8_t maxlen, int32_t value);
void cmn_utility_int2strdec( char *str, uint8_t maxlen, int32_t value);
void cmn_utility_uint2strhex( char *str, uint8_t maxlen, uint32_t value);
void cmn_utility_uint2strdec( char *str, uint8_t maxlen, uint32_t value);

char *cmn_utility_strrev(char *str);
char *cmn_utility_strnrev(char *str, size_t len);

void cmn_utility_angleinc( uint16_t *hour_inc, uint16_t *minute_inc, uint16_t NULLABLE *second_inc, uint32_t ms);
void cmn_utility_angleset( uint16_t *hour_deg, uint16_t *minute_deg, uint16_t NULLABLE *second_deg, const cmnDateTime_t *pTime);
void cmn_utility_timeinc( cmnDateTime_t *pTime, uint32_t ms);

cmnDateTime_t cmn_utility_set_time_from_iso( const char* __timestamp__);

#ifdef __cplusplus
}
#endif

#endif