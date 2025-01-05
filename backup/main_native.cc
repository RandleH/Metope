
#include <stdio.h>
#include "cmn_utility.h"

#include "trace.h"

int main(int argc, char *argv[]){
  TRACE_INFO("2025/01/01 21:48:9\n");

  cmnDateTime_t time = {
    .word = 3302002753
  };
  TRACE_INFO("Time: %u/%u/%u %u:%u:%u\n", time.year+2024, time.month, time.day, time.hour, time.minute, time.second);

  uint16_t hour_rem   = 11979;
  uint16_t minute_rem = 979;
  uint16_t hour_deg   = 599;
  uint16_t minute_deg = 3599;
  uint32_t ms         = 75;

  uint16_t hour_inc, minute_inc;
  cmn_utility_angleinc( &hour_rem, &minute_rem, NULL, &hour_inc, &minute_inc, NULL, ms);
  minute_deg += minute_inc;
  hour_deg   += hour_inc;

  {
    uint32_t minute_in_ms     = cmn_utility_mindeg2_ms(minute_deg);
    uint32_t delta_hour_in_ms = cmn_utility_hrdeg2_ms((hour_deg%(3600/12)));

    uint32_t minute_in_ms_total     = minute_in_ms + minute_rem;
    uint32_t delta_hour_in_ms_total = delta_hour_in_ms + hour_rem;
    TRACE_DEBUG("ms=%u H_rem=%u M_rem=%u H_deg=%u M_deg=%u h'_ms=%u m_ms=%u H'_ms=%u M_ms=%u\n", \
      ms,
      hour_rem,\
      minute_rem,\
      hour_deg,\
      minute_deg,\
      delta_hour_in_ms,\
      minute_in_ms,\
      delta_hour_in_ms_total,\
      minute_in_ms_total
    );
    if(minute_in_ms_total != delta_hour_in_ms_total && minute_in_ms_total%3600000 != delta_hour_in_ms_total ){
      TRACE_ERROR("Needle Pattern Mismatched\n");
    }
  }

  return 0;
}