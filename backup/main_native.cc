
#include <stdio.h>
#include "cmn_utility.h"
#include "app_cmdbox.h"
// #include "/Users/randleh/GitHub/Metope_cmdbox/app/app_cmdbox.c"
#include "trace.h"


int main(int argc, char *argv[]){
  TRACE_INFO("Hello world");

  app_cmdbox_parse("DISPOFF\r");
  app_cmdbox_parse("DISPON\r");
  app_cmdbox_parse("CCW 1\r");
  app_cmdbox_parse("CW  3\r");
  app_cmdbox_parse("HEL 1\r");
  
  return 0;
}

/**
 * @note Misc `cmnDateTime_t` | `cmn_utility_timediff()`
 */
int main_template_1(int argc, char *argv[]){

  cmnDateTime_t timeA = {
    .year   = 0,
    .month  = 1,
    .day    = 12,
    .hour   = 17,
    .minute = 59,
    .second = 59
  };
  TRACE_INFO("TimeA: %u/%u/%u %u:%u:%u\n", timeA.year+2024, timeA.month, timeA.day, timeA.hour, timeA.minute, timeA.second);

  cmnDateTime_t timeB = {
    .year   = 0,
    .month  = 1,
    .day    = 12,
    .hour   = 18,
    .minute = 0,
    .second = 0
  };
  TRACE_INFO("TimeB: %u/%u/%u %u:%u:%u\n", timeB.year+2024, timeB.month, timeB.day, timeB.hour, timeB.minute, timeB.second);


  TRACE_INFO("TimeA - TimeB = %d\n", cmn_utility_timediff(timeA, timeB));

  return 0;
}

/**
 * @note Misc `cmn_utility_angleinc()`, `cmn_utility_mindeg2_ms()`, `cmn_utility_hrdeg2_ms()`
 */
int main_template_2(int argc, char *argv[]){
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