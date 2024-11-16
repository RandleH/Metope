


#include "lvgl.h"
#ifndef APP_TYPE_H
#define APP_TYPE_H

#define APP_CFG_TASK_SCREEN_FRESH_STACK_SIZE (400U)

typedef enum AppTaskPriority{
  kAppPriority_VERY_IMPORTANT = 50,
  kAppPriority_IMPORTANT      = 40,
  kAppPriority_INTERMEDIATE   = 30,
  kAppPriority_NORMAL         = 20,
  kAppPriority_DOCUMENTATION  = 5
}AppTaskPriority_t;

typedef struct{
  lv_obj_t *pScreen;
  lv_obj_t *pPinHour;
  lv_obj_t *pPinMinute;

  uint16_t _degree_hour;
  uint16_t _degree_minute;

  void     *p_anything;
} tAppGuiClockParam;


/**
 * @brief List of all supported clock ui
 */
typedef enum{
  kAppGuiClock_None   = 255,
  kAppGuiClock_Clock1 = 0
} AppGuiClockEnum_t;


#endif