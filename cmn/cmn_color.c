/**
 ******************************************************************************
 * @file    cmn_color.c
 * @author  RandleH
 * @brief   Color RGB / Grayscale / Binary
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
#include "cmn_color.h"


#ifdef __cplusplus
extern "C"{
#endif


/**
 * @brief Calculate the color gradient in linear mapping
 * @param [in] color_from - Source Color
 * @param [in] color_to   - Destination Color
 * @param [in] x          - Value in the scale of [0~255]
 * @return Return mapped color value
 */
cmnColor_t cmn_color_gradient( cmnColor_t color_from, cmnColor_t color_to, uint8_t x){
  uint8_t r = (GET_R(color_from) <= GET_R(color_to))                        ?\
              GET_R(color_from)-((GET_R(color_from)-GET_R(color_to))*x >> 8):\
              GET_R(color_from)+((GET_R(color_to)-GET_R(color_from))*x >> 8);
  uint8_t g = (GET_G(color_from) <= GET_G(color_to))                        ?\
              GET_G(color_from)-((GET_G(color_from)-GET_G(color_to))*x >> 8):\
              GET_G(color_from)+((GET_G(color_to)-GET_G(color_from))*x >> 8);
  uint8_t b = (GET_B(color_from) <= GET_B(color_to))                        ?\
              GET_B(color_from)-((GET_B(color_from)-GET_B(color_to))*x >> 8):\
              GET_B(color_from)+((GET_B(color_to)-GET_B(color_from))*x >> 8);
  return cmn_color_make(r,g,b);
}

#ifdef __cplusplus
}
#endif // __cplusplus

/* ********************************** EOF *********************************** */
