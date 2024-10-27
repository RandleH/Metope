


#include <stdint.h>
#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  #include "stm32f4xx_hal.h"
#endif

#ifndef CMN_TYPE_H
#define CMN_TYPE_H




#define ENABLE    (1U)
#define DISABLE   (0U)
#define ON        (1U)
#define OFF       (0U)
#define YES       (1U)
#define NO        (0U)
#define NULLABLE
#define FAILED    (0U)
#define PASSED    (1U)

#ifdef TEST_ONLY
  #define STATIC
  #define INLINE
#else
  #define STATIC static
  #define INLINE inline
#endif



#ifdef __cplusplus
extern "C"{
#endif

typedef uint8_t cmnBoolean_t;

#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
typedef IRQn_Type cmnIRQn_t;
#endif


typedef struct cmnDateTime_t{
  struct{
    uint32_t  year   : 6;    /*!< Distance from 2024 +- 32 */
    uint32_t  month  : 4;
    uint32_t  day    : 5;
    uint32_t  hour   : 5;
    uint32_t  minute : 6;
    uint32_t  second : 6;
  };
  uint32_t word;
}cmnDateTime_t;




#ifdef __cplusplus
}
#endif  // extern C
#endif  // CMN_TEST_H

