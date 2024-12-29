/**
 ******************************************************************************
 * @file    assert.h
 * @author  RandleH
 * @brief   Project Assertion
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

#ifndef ASSERT_H
#define ASSERT_H



#ifdef DEBUG
#ifdef __cplusplus
extern "C"{
#endif
void assert( bool expr, const char *msg, uint32_t line, const char *filename);
#ifdef __cplusplus
}
#endif
#define ASSERT( expr, msg) assert( (expr), (msg), __LINE__, __FILE__)
#else
#define ASSERT( expr, msg)
#endif









#endif