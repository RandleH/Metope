


static int app_cmdbox_callback_1args_CCW(const char *cmd, ...) {
#if (defined SYS_TARGET_NATIVE)
  TRACE_DEBUG("\tExecute user command: %s", cmd);
#elif (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
#endif
  return 0;
}
static int app_cmdbox_callback_1args_CW(const char *cmd, ...) {
#if (defined SYS_TARGET_NATIVE)
  TRACE_DEBUG("\tExecute user command: %s", cmd);
#elif (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
#endif
  return 0;
}
static int app_cmdbox_callback_0args_DISPOFF(const char *cmd, ...) {
#if (defined SYS_TARGET_NATIVE)
  TRACE_DEBUG("\tExecute user command: %s", cmd);
#elif (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  xEventGroupSetBits( metope.app.rtos.event._handle, CMN_EVENT_SCREEN_DISPOFF);
#endif
  return 0;
}
static int app_cmdbox_callback_0args_DISPON(const char *cmd, ...) {
#if (defined SYS_TARGET_NATIVE)
  TRACE_DEBUG("\tExecute user command: %s", cmd);
#elif (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  xEventGroupSetBits( metope.app.rtos.event._handle, CMN_EVENT_SCREEN_DISPON);
#endif
  return 0;
}
static int app_cmdbox_callback_0args_GT(const char *cmd, ...) {
#if (defined SYS_TARGET_NATIVE)
  TRACE_DEBUG("\tExecute user command: %s", cmd);
#elif (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
#endif
  return 0;
}
static int app_cmdbox_callback_6args_ST(const char *cmd, ...) {
#if (defined SYS_TARGET_NATIVE)
  TRACE_DEBUG("\tExecute user command: %s", cmd);
#elif (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
#endif
  return 0;
}
