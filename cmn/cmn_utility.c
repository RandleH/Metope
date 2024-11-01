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





#include "cmn_utility.h"
#include <stdlib.h>



#ifdef __cplusplus
extern "C"{
#endif

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



#ifdef __cplusplus
}
#endif

