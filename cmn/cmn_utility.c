/**
 ******************************************************************************
 * @file    cmn_utility.c
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
#include "cmn_utility.h"
#include "cmn_math.h"
#include <stdlib.h>
#include <string.h>


#ifdef __cplusplus
extern "C"{
#endif


/* ************************************************************************** */
/*                 Utility Functions: Convert string to number                */
/* ************************************************************************** */
int32_t cmn_utility_str2dec(const char *str, cmnBoolean_t *isValid){
  char  *eptr;
  int32_t ret = strtol(str, &eptr, 10);
  if(isValid!=NULL){
    *isValid = ((*eptr)=='\0');
  }
  return ret;
}



int32_t cmn_utility_str2hex(const char *str, cmnBoolean_t *isValid){
  char  *eptr;
  int32_t ret = strtol(str, &eptr, 16);
  if(isValid!=NULL){
    *isValid = ((*eptr)=='\0');
  }
  return ret;
}



float cmn_utility_str2float(const char *str, cmnBoolean_t *isValid){
  char  *eptr;
  float ret = strtof( str, &eptr);
  if(isValid!=NULL){
    *isValid = ((*eptr)=='\0');
  }
  return ret;
}


/* ************************************************************************** */
/*                   Utility Functions: String Manipulation                   */
/* ************************************************************************** */
char *cmn_utility_strrev(char *str){
  return cmn_utility_strnrev(str, strlen(str));
}


char *cmn_utility_strnrev(char *str, size_t len){
  size_t nel = 0;
  --len;
  while(len>nel){
    CMN_SWAP(str[len], str[nel]);
    --len; ++nel;
  }
  return str;
}


/* ************************************************************************** */
/*                 Utility Functions: Convert number to string                */
/* ************************************************************************** */
/**
 * @brief Insert `#` instead of digits indicating this number can NOT fit in the width
 * @param [out] str     - String buffer
 * @param [in]  maxlen  - The maximum lenth of this string buffer
 * @param [in]  wordlen - The length of words
 * @return Return how many `#` have been inserted
 */
static uint8_t cmn_utility_invalidify_str( char *str, uint8_t maxlen, uint8_t wordlen){
  uint8_t idx = CMN_MIN( maxlen-1, wordlen);
  memset( str, '#', idx);
  str[idx] = '\0';
  return idx;
}

/**
 * @brief Convert an unsigned integer to string in Decimal with width option
 * 
 * @param [out] str    - String buffer
 * @param [in]  maxlen - The maximum lenth of this string buffer
 * @param [in]  value  - Input
 * @param [in]  width  - The Starting pow 10 divisor for value
 * @return Return num of characters that have been placed to the buffer
 */
uint8_t cmn_utility_uint2strdec_width( char *str, uint8_t maxlen, uint32_t value, uint8_t width){
  if(maxlen==0 || !str) return 0;
  
  uint8_t wordlen = cmn_math_count_dec_digits(value);

  if( width==0xFF ){
    width = wordlen;
  }

  if(likely( wordlen <= width && width <= maxlen-1 )){
    /**
     * @note
     *  Insert the character through the LSB order
     */
    uint8_t idx = width;
    div_t   tmp = {.quot = value};
    str[idx] = '\0';

    do{
      tmp = div(tmp.quot, 10);
      str[--idx] = '0'+tmp.rem;

    }while(idx > 0);
    return width;
  }else{
    /**
     * @note
     *  The number string can NOT fit in either the maximum length or the given width
     */
    return cmn_utility_invalidify_str( str, maxlen, wordlen);
  }
}

/**
 * @brief Convert a signed integer to string in Decimal with width option
 * 
 * @param [out] str    - String buffer
 * @param [in]  maxlen - The maximum lenth of this string buffer
 * @param [in]  value  - Input
 * @param [in]  width  - The Starting pow 10 divisor for value
 * @return Return num of characters that have been placed to the buffer
 */
uint8_t cmn_utility_int2strdec_width( char *str, uint8_t maxlen, int32_t value, uint8_t width){
  if(value<0 && maxlen!=0){
    str[0] = '-';
    return 1 + cmn_utility_uint2strdec_width( ++str, maxlen-1, -value, width);
  }else{
    return cmn_utility_uint2strdec_width( str, maxlen, value, width);
  }
}

/**
 * @brief Convert an unsigned integer to string in Hexadecimal with width option
 * 
 * @param [out] str    - String buffer
 * @param [in]  maxlen - The maximum lenth of this string buffer
 * @param [in]  value  - Input
 * @param [in]  width  - Word length for the number. Give `0xFF` will take whatever the string length the value will take.
 * @return Return num of characters that have been placed to the buffer
 */
uint8_t cmn_utility_uint2strhex_width( char *str, uint8_t maxlen, uint32_t value, uint8_t width){
  if(maxlen==0 || !str) 
    return 0;
  
  uint8_t wordlen = value==0 ? 1 : (8-(__builtin_clz(value)>>2));

  if( width==0xFF ){
    width = wordlen;
  }

  if(likely( wordlen <= width && width <= maxlen-1 )){
    /**
     * @note
     *  Insert the character through the LSB order
     */
    uint8_t idx = width;
    str[idx] = '\0';
    do{
      char c_offset = (value & 0x0F);
      str[--idx] = c_offset < 10 ? '0'+c_offset : 'A'+(c_offset-0x0A);
      value >>= 4;
    }while(idx > 0);
    return width;
  }else{
    /**
     * @note
     *  The number string can NOT fit in either the maximum length or the given width
     */
    return cmn_utility_invalidify_str( str, maxlen, wordlen);
  }
}

/**
 * @brief Convert a signed integer to string in Decimal with width option
 * 
 * @param [out] str    - String buffer
 * @param [in]  maxlen - The maximum lenth of this string buffer
 * @param [in]  value  - Input
 * @param [in]  width  - The Starting pow 10 divisor for value
 * @return Return num of characters that have been placed to the buffer
 */
uint8_t cmn_utility_int2strhex_width( char *str, uint8_t maxlen, int32_t value, uint8_t width){
  if(value<0 && maxlen!=0){
    str[0] = '-';
    return 1 + cmn_utility_uint2strhex_width( ++str, maxlen-1, -value, width);
  }else{
    return cmn_utility_uint2strhex_width( str, maxlen, value, width);
  }
}

/**
 * @brief Convert an unsigned integer to string in Decimal
 * 
 * @param [out] str            - String buffer
 * @param [in]  maxlen         - The maximum lenth of this string buffer
 * @param [in]  value          - Input
 * @return Return num of characters that have been placed to the buffer
 */
uint8_t cmn_utility_uint2strdec( char *str, uint8_t maxlen, uint32_t value){
  return cmn_utility_uint2strdec_width( str, maxlen, value, (uint8_t)-1);
}

/**
 * @brief Convert an unsigned integer to string in Hexadecimal
 * 
 * @param [out] str    - String buffer
 * @param [in]  maxlen - The maximum lenth of this string buffer
 * @param [in]  value  - Input
 * @return Return num of characters that have been placed to the buffer
 */
uint8_t cmn_utility_uint2strhex( char *str, uint8_t maxlen, uint32_t value){
  return cmn_utility_uint2strhex_width( str, maxlen, value, (uint8_t)-1);
}

/**
 * @brief Convert a signed integer to string in Decimal
 * 
 * @param [out] str    - String buffer
 * @param [in]  maxlen - The maximum lenth of this string buffer
 * @param [in]  value  - Input
 * @return Return num of characters that have been placed to the buffer
 */
uint8_t cmn_utility_int2strdec( char *str, uint8_t maxlen, int32_t value){
  return cmn_utility_int2strdec_width( str, maxlen, value, (uint8_t)-1);
}

/**
 * @brief Convert a signed integer to string in Hexadecimal
 * 
 * @param [out] str    - String buffer
 * @param [in]  maxlen - The maximum lenth of this string buffer
 * @param [in]  value  - Input
 * @return Return num of characters that have been placed to the buffer
 */
uint8_t cmn_utility_int2strhex( char *str, uint8_t maxlen, int32_t value){
  return cmn_utility_int2strhex_width( str, maxlen, value, (uint8_t)-1);
}


/* ************************************************************************** */
/*              Utility Functions: Clock Needle Angle Calculation             */
/* ************************************************************************** */

/**
 * @brief   Calculate how much angle need to change given a microsecond increase
 * @note    The angle degreee was in scale of [0:3599]
 * @param [inout] hour_rem    - Remaining angle for hour
 * @param [inout] minute_rem  - Remaining angle for minute
 * @param [inout] second_rem  - Remaining angle for second
 * @param [out]   hour_inc    - Increased angle for hour
 * @param [out]   minute_inc  - Increased angle for minute
 * @param [out]   second_inc  - Increased angle for second (optional)
 * @param [in]    ms          - The increased microseconds
 */
void cmn_utility_angleinc( 
                          uint16_t          *hour_rem  ,\
                          uint16_t          *minute_rem,\
                          uint16_t NULLABLE *second_rem,\
                          uint16_t          *hour_inc  ,\
                          uint16_t          *minute_inc,\
                          uint16_t NULLABLE *second_inc,\
                          uint32_t           ms\
                          ){
  div_t remainder_hour   = {.rem = *hour_rem};
  div_t remainder_minute = {.rem = *minute_rem};
  div_t remainder_second = {.rem = second_rem==NULL ? 0: *second_rem};

#if 0
  remainder_hour = div( remainder_hour.rem + ms*3600, 43200000);
  remainder_minute = div( remainder_minute.rem + ms*3600, 3600000);  // It takes `3600000` ms to complete a secondwise circle
  *hour_inc = remainder_hour.quot;
  *minute_inc = remainder_minute.quot;
  if(NULL!=second_inc){
    remainder_second = div( remainder_second.rem + ms*3600, 60000);  // It takes `60000` ms to complete a secondwise circle
    *second_inc = remainder_second.quot;
  }
#else
  remainder_hour = div( remainder_hour.rem + ms, 12000);
  remainder_minute = div( remainder_minute.rem + ms, 1000);
  *hour_inc = remainder_hour.quot;
  *minute_inc = remainder_minute.quot;
  if(NULL!=second_inc){
    remainder_second = div( remainder_second.rem + ms*3, 50);
    *second_inc = remainder_second.quot;
  }
#endif
  return;
}


/**
 * @brief   Calculate the absolute angle degree per clock unit
 * @note    The angle degreee was in scale of [0:3599]
 * @param [out] hour_inc    - Absolute angle for hour
 * @param [out] minute_inc  - Absolute angle for minute
 * @param [out] second_inc  - Absolute angle for second (optional)
 * @param [in]  pTime       - Current time
 */
void cmn_utility_angleset( uint16_t *hour_deg, uint16_t *minute_deg, uint16_t NULLABLE *second_deg, const cmnDateTime_t *pTime){
  uint8_t hour = pTime->hour>12 ? pTime->hour-12 : pTime->hour;
  
  *hour_deg   =        hour  *300 + pTime->minute*5 + pTime->second/12;
  *minute_deg = pTime->minute*60  + pTime->second;
  if(NULL!=second_deg){
    *second_deg = pTime->second * 60;
  }
}






/* ************************************************************************** */
/*                    Utility Functions: Timing Calculation                   */
/* ************************************************************************** */
/**
 * @todo: Add CI for this function
 */
/**
 * @brief   Update the time given a increased microsecond
 * @warning Always pass the same time pointer to this function.Becasuse this function will
 *          record the remaining ms from the last call.
 * @param [inout] pTime - Time object to update
 * @param [in]    ms    - The increased microseconds
 */
void cmn_utility_timeinc( cmnDateTime_t *pTime, uint32_t ms){
  static div_t local = {0};

  local = div( local.rem+ms,1000);

  div_t tmp;
  if(unlikely(pTime->second+local.quot >= 60)){
    tmp = div(pTime->second+local.quot, 60);
    pTime->second = tmp.rem;

    if(unlikely(pTime->minute+tmp.quot >= 60)){
      tmp = div(pTime->minute+tmp.quot, 60);
      pTime->minute = tmp.rem;

      if(unlikely(pTime->hour+tmp.quot >= 24)){
        tmp = div(pTime->hour+tmp.quot, 24);
        pTime->hour = tmp.rem;

        uint8_t max_day_this_month = ((pTime->month <=7) ? (30 + (pTime->month&0x01)) : (31 - (pTime->month&0x01)));

        /**
         * @note: Special case for leap year
         */
        if(unlikely(pTime->year%4==0 && pTime->month==2)){
          max_day_this_month = 28;
        }
        if(unlikely(pTime->day+tmp.quot >= max_day_this_month)){
          tmp = div(pTime->day+tmp.quot, max_day_this_month);
          pTime->day = tmp.rem;
          
          if(unlikely(pTime->month+tmp.quot>12)){
            tmp = div(pTime->month+tmp.quot, 12);
            pTime->month = tmp.rem;
            pTime->year+=tmp.quot;
          }
          else{
            pTime->month += tmp.quot;
          }
        }
        else{
          pTime->day += tmp.quot;
        }
      }
      else{
        pTime->hour += tmp.quot;
      }
    }
    else{
      pTime->minute += tmp.quot;
    }
  }
}

/**
 * @brief Assign the datetime from an ISO string 
 * @param [in] __timestamp__  An ISO string for time representation. Use C/C++ `__TIMESTAMP__`.
 * @example
 *  "Thu Dec  5 23:47:39 2024"
 */
cmnDateTime_t cmn_utility_set_time_from_iso( const char* __timestamp__){
  cmnDateTime_t result = {
    .year   = (__timestamp__[20]-'0')*1000 + (__timestamp__[21]-'0')*100 + (__timestamp__[22]-'0')*10 + (__timestamp__[23]-'0') - 2024,
    .month  = 0, // @todo: Fix the correct month
    .day    = (__timestamp__[8]-'0')*10 + (__timestamp__[9]-'0'),
    .hour   = (__timestamp__[11]-'0')*10 + (__timestamp__[12]-'0'),
    .minute = (__timestamp__[14]-'0')*10 + (__timestamp__[15]-'0'),
    .second = (__timestamp__[17]-'0')*10 + (__timestamp__[18]-'0')
  };

  switch(__timestamp__[4]+__timestamp__[5]+__timestamp__[6]){
    case 'J'+'a'+'n':{
      result.month = 1;
      break;
    }
    case 'F'+'e'+'b':{
      result.month = 2;
      break;
    }
    case 'M'+'a'+'r':{
      result.month = 3;
      break;
    }
    case 'A'+'p'+'r':{
      result.month = 4;
      break;
    }
    case 'M'+'a'+'y':{
      result.month = 5;
      break;
    }
    case 'J'+'u'+'n':{
      result.month = 6;
      break;
    }
    case 'J'+'u'+'l':{
      result.month = 7;
      break;
    }
    case 'A'+'u'+'g':{
      result.month = 8;
      break;
    }
    case 'S'+'e'+'p':{
      result.month = 9;
      break;
    }
    case 'O'+'c'+'t':{
      result.month = 10;
      break;
    }
    case 'N'+'o'+'v':{
      result.month = 11;
      break;
    }
    case 'D'+'e'+'c':{
      result.month = 12;
      break;
    }
    default:{
      result.month = 0; // Impossible
      break;
    }
  }
  return result;
}

#ifdef __cplusplus
}
#endif

