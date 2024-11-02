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



#include "cmn_math.h"




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


