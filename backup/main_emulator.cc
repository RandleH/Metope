/**
 ******************************************************************************
 * @file    main.cc
 * @author  RandleH
 * @brief   Unit Testing
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

#include <stdio.h>
#include "init.h"
#include "trace.h"

#ifdef __cplusplus
extern "C"{
#endif


int main(int argc, char *argv[]){
  hw_init();
  TRACE_INFO("System boot completed.");
  
  return 1;
}

#ifdef __cplusplus
}
#endif