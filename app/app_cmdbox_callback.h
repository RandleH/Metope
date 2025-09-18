
#include <stdarg.h>
#include "bsp_screen.h"
#include "bsp_rtc.h"

#if !defined(UNUSED)
#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#endif /* UNUSED */

static int app_cmdbox_callback_1args_CCW(const char *cmd, ...) {
  va_list args;
  va_start(args, cmd);
#if (defined SYS_TARGET_NATIVE)
  TRACE_DEBUG("\tExecute user command: %s", cmd);
#elif (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || defined (EMULATOR_STM32F405RGT6)
  int delta = va_arg(args, int);
  bsp_screen_rotate( delta, 1);
#endif
  va_end(args);
  return 0;
}
static int app_cmdbox_callback_1args_CW(const char *cmd, ...) {
  va_list args;
  va_start(args, cmd);
#if (defined SYS_TARGET_NATIVE)
  TRACE_DEBUG("\tExecute user command: %s", cmd);
#elif (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || defined (EMULATOR_STM32F405RGT6)
  int delta = va_arg(args, int);
  bsp_screen_rotate( delta, 0);
#endif
  va_end(args);
  return 0;
}
static int app_cmdbox_callback_1args_DISPBR(const char *cmd, ...) {
  va_list args;
  va_start(args, cmd);
#if (defined SYS_TARGET_NATIVE)
  TRACE_DEBUG("\tExecute user command: %s", cmd);
#elif (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || defined (EMULATOR_STM32F405RGT6)
  int brightness_100 = va_arg(args, int);
  metope.bsp.screen.brightness = (((brightness_100 % 100) << 11) / 100);
  xEventGroupSetBits( metope.rtos.event._handle, CMN_EVENT_SCREEN_DISPBR);
  #endif
  va_end(args);
  return 0;
}
static int app_cmdbox_callback_0args_DISPOFF(const char *cmd, ...) {
#if (defined SYS_TARGET_NATIVE)
  TRACE_DEBUG("\tExecute user command: %s", cmd);
#elif (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || defined (EMULATOR_STM32F405RGT6)
  xEventGroupSetBits( metope.rtos.event._handle, CMN_EVENT_SCREEN_DISPOFF);
#endif
  return 0;
}
static int app_cmdbox_callback_0args_DISPON(const char *cmd, ...) {
#if (defined SYS_TARGET_NATIVE)
  TRACE_DEBUG("\tExecute user command: %s", cmd);
#elif (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || defined (EMULATOR_STM32F405RGT6)
  xEventGroupSetBits( metope.rtos.event._handle, CMN_EVENT_SCREEN_DISPON);
#endif
  return 0;
}
static int app_cmdbox_callback_0args_GT(const char *cmd, ...) {
#if (defined SYS_TARGET_NATIVE)
  TRACE_DEBUG("\tExecute user command: %s", cmd);
#elif (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || defined (EMULATOR_STM32F405RGT6)
  cmnDateTime_t time = bsp_rtc_get_time();
  TRACE_INFO("=> %02d:%02d:%02d %02d/%02d/%04d", time.hour, time.minute, time.second, time.month, time.day, time.year + CMN_DATE_YEAR_OFFSET);
  UNUSED(time);
#endif
  return 0;
}
static int app_cmdbox_callback_6args_ST(const char *cmd, ...) {
  va_list args;
  va_start(args, cmd);
#if (defined SYS_TARGET_NATIVE)
  TRACE_DEBUG("\tExecute user command: %s", cmd);
#elif (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || defined (EMULATOR_STM32F405RGT6)
  cmnDateTime_t time = {0};
  time.year   = va_arg(args, int) - CMN_DATE_YEAR_OFFSET;
  time.month  = va_arg(args, int);
  time.day    = va_arg(args, int);
  time.hour   = va_arg(args, int);
  time.minute = va_arg(args, int);
  time.second = va_arg(args, int);
  bsp_rtc_set_time(time);
#endif
  va_end(args);
  return 0;
}
