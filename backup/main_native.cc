
#include <stdio.h>
#include "cmn_utility.h"

int main(int argc, char *argv[]){
  uint16_t hour_rem;  
  uint16_t minute_rem;
  uint16_t second_rem;
  uint16_t hour_deg;  
  uint16_t minute_deg;
  uint16_t second_deg;

  printf("2025/01/01 13:24:37\n");
  
  cmnDateTime_t time = {
    .year   = 1,
    .month  = 1,
    .day    = 1,
    .hour   = 13,
    .minute = 24,
    .second = 37
  };
  cmn_utility_angleset( &hour_rem, &minute_rem, &second_rem, &hour_deg, &minute_deg, &second_deg, &time);

  printf( "H_rem=%u M_rem=%u H_deg=%u M_deg=%u\n", hour_rem, minute_rem, hour_deg, minute_deg );

  uint32_t minute_in_ms     = cmn_utility_mindeg2_ms(minute_deg);
  uint32_t delta_hour_in_ms = cmn_utility_hrdeg2_ms((hour_deg%(3600/12)));
  uint32_t minute_in_ms_total     = minute_in_ms + minute_rem;
  uint32_t delta_hour_in_ms_total = delta_hour_in_ms + hour_rem;

  printf("h'_ms=%u m_ms=%u H'_ms=%u M_ms=%u\n", delta_hour_in_ms, minute_in_ms, delta_hour_in_ms_total, minute_in_ms_total);
  return 0;
}