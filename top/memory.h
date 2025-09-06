/**
 ******************************************************************************
 * @file    memory.h
 * @author  RandleH
 * @brief   Memory Interface Layer
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

#ifndef MEMORY_H
#define MEMORY_H

/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include <stddef.h>


#ifdef __cplusplus
extern "C"{
#endif

void *MALLOC(size_t xWantedSize);
void FREE(void *pv);
void *REALLOC(void *pv, size_t xWantedSize);


#ifdef __cplusplus
}
#endif
#endif
