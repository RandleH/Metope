/**
 ******************************************************************************
 * @file    cmn_math.c
 * @author  RandleH
 * @brief   Common Program - Math
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
#include "device.h"
#include "cmn_math.h"



#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Find the greatest common divisor
 * @note This function is recursive.
 * @param [in] a - Input
 * @param [in] b - Input
 * @return Return gcd.
 */
uint32_t cmn_math_gcd( uint32_t a, uint32_t b){
  if( b>a )  return cmn_math_gcd(b,a);
  
  if( b==0 ) return a;
  
  if( !(a&1) ){
    if( !(b&1) ) return (uint32_t)(cmn_math_gcd(a>>1, b>>1) <<1);
    else         return (uint32_t)(cmn_math_gcd(a>>1, b   )    );
  }else{
    if( !(b&1) ) return (uint32_t)(cmn_math_gcd(a   , b>>1)    );
    else         return (uint32_t)(cmn_math_gcd(a-b , b   )    );
  }
  return 0;
}

/**
 * @brief Find the nearest pow of 2 value.
 * @note For example: `0b00110010` => `0b00100000` | `0b0001` => `0b0001` | `0b0000` => `0b0000`
 * @param [in] x - Input value
 * @return Return an positive integer pow of 2, which means ( x%2==0 )
 * @addtogroup MachineDependent
 */
uint32_t cmn_math_largest_pow2(uint32_t x){
  if( x==0 ) return 0;
#if 0
  x--;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x++;
  return x;
#else
  return (1<<(32-__builtin_clz(x)-1));
#endif
}



const uint32_t TABLE_POW10[] = {
  1,           // 00000000000000000000000000000001
  10,          // 00000000000000000000000000001010
  100,         // 00000000000000000000000001100100
  1000,        // 00000000000000000000001111101000
  10000,       // 00000000000000000010011100010000
  100000,      // 00000000000000011000011010100000
  1000000,     // 00000000000011110100001001000000
  10000000,    // 00000000100110001001011010000000
  100000000,   // 00000101111101011110000100000000
  1000000000,  // 00111011100110101100101000000000
};

/**
 * @brief Return the pow of 10
 * @note O(1)
 * @param [in] x - Input
 * @return Always return `1000000000` when x>9 cuz it's too big. 
 */
uint32_t cmn_math_pow10(uint8_t x){
  if( x > 9 ){
    x = 9; // Overflow
  }
  return TABLE_POW10[x];
}

/**
 * @brief Find the nearest pow of 10 value.
 * 
 * @note For example: `12` => `10` | `1234` => `1000` | `0` => `0` | `1` => `1`
 * @param [in] x - Input value
 * @return Return an positive integer pow of 10, which means ( x%10==0 )
 */
uint32_t cmn_math_largest_pow10(uint32_t x){
  if(x==0) return 0;
  
  int8_t l=0,r=sizeof(TABLE_POW10)/sizeof(*TABLE_POW10),m;
  while(r-l!=1){
    m=(l+r)>>1;
    if(TABLE_POW10[m] < x){
      l = m;
    }else if(TABLE_POW10[m] > x){
        r = m;
    }else{
      return TABLE_POW10[m];
    }
  }
  return TABLE_POW10[l];
}


/**
 * @brief Count how many digits were valid in decimal.
 * @param [in] x - Input
 * @return Return how many digits were valid; `0` will have 1 valid digit.
 */
uint8_t cmn_math_count_dec_digits( uint32_t x){
  if(x==0){
    return 1;
  }
  
  int8_t l=0,r=sizeof(TABLE_POW10)/sizeof(*TABLE_POW10),m;
  while(r-l!=1){
    m=(l+r)>>1;
    if(TABLE_POW10[m] < x){
      l = m;
    }else if(TABLE_POW10[m] > x){
        r = m;
    }else{
      return m+1;
    }
  }
  return l+1;
}


#ifdef __cplusplus
}
#endif
/* ********************************** EOF *********************************** */
