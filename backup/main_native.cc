
#include <stdio.h>
#include "cmn_utility.h"

#include "trace.h"

int main(int argc, char *argv[]){
  uint16_t hour_rem;  
  uint16_t minute_rem;
  uint16_t second_rem;
  uint16_t hour_deg;  
  uint16_t minute_deg;
  uint16_t second_deg;

  

  TRACE_INFO("2025/01/01 21:48:9\n");


  TRACE_DEBUG("Message Level: %s\n", "Debug");
  TRACE_INFO("Message Level: %s\n", "Info");
  TRACE_WARNING("Message Level: %s\n", "Warning");
  TRACE_ERROR("Message Level: %s\n", "Error");

  cmnDateTime_t time = {
    .year   = 1,
    .month  = 1,
    .day    = 1,
    .hour   = 21,
    .minute = 48,
    .second = 9
  };
  cmn_utility_angleset( &hour_rem, &minute_rem, &second_rem, &hour_deg, &minute_deg, &second_deg, &time);

  uint32_t minute_in_ms     = cmn_utility_mindeg2_ms(minute_deg);
  uint32_t delta_hour_in_ms = cmn_utility_hrdeg2_ms((hour_deg%(3600/12)));
  uint32_t minute_in_ms_total     = minute_in_ms + minute_rem;
  uint32_t delta_hour_in_ms_total = delta_hour_in_ms + hour_rem;

  printf("H_rem=%u M_rem=%u H_deg=%u M_deg=%u h'_ms=%u m_ms=%u H'_ms=%u M_ms=%u\n", hour_rem, minute_rem, hour_deg, minute_deg, delta_hour_in_ms, minute_in_ms, delta_hour_in_ms_total, minute_in_ms_total);

  int cnt = 128;
  while(cnt--){
    uint16_t hour_inc, minute_inc, second_inc;
    cmn_utility_angleinc( &hour_rem, &minute_rem, &second_rem, &hour_inc, &minute_inc, &second_inc, 64);
    hour_deg   += hour_inc;
    minute_deg += minute_inc;
    second_deg += second_inc;

    minute_in_ms           = cmn_utility_mindeg2_ms(minute_deg);
    delta_hour_in_ms       = cmn_utility_hrdeg2_ms((hour_deg%(3600/12)));
    minute_in_ms_total     = minute_in_ms + minute_rem;
    delta_hour_in_ms_total = delta_hour_in_ms + hour_rem;
    if(delta_hour_in_ms_total!=minute_in_ms_total){
      printf(">");
    }
    printf( "H_rem=%u M_rem=%u H_deg=%u M_deg=%u h'_ms=%u m_ms=%u H'_ms=%u M_ms=%u\n", hour_rem, minute_rem, hour_deg, minute_deg, delta_hour_in_ms, minute_in_ms, delta_hour_in_ms_total, minute_in_ms_total);
  }
  return 0;
}