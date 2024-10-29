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





#ifndef CMN_UTILITY_H
#define CMN_UTILITY_H



#define CMN_SWAP( a, b)   do{ (a)^=(b); (b)^=(a); (a)^=(b); }while(0)

#define CMN_MIN( a, b)    ((a)>(b) ? (b) : (a))
#define CMN_MAX( a, b)    ((a)<(b) ? (b) : (a))


#endif