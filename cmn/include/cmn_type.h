


#include <stdint.h>


#ifndef CMN_TYPE_H
#define CMN_TYPE_H





#define ON      (1U)
#define OFF     (0U)
#define YES     (1U)
#define NO      (0U)



typedef uint8_t cmnBoolean_t;

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





#endif
