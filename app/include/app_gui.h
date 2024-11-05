

#include "lvgl.h"
#include "cmn_type.h"

#ifndef APP_GUI_H
#define APP_GUI_H


#ifdef __cplusplus
extern "C"{
#endif


typedef struct{
  lv_obj_t *pScreen;
  lv_obj_t *pPinHour;
  lv_obj_t *pPinMinute;

  cmnDateTime_t time;
} tGuiClock1;

void ui_Clock1_screen_init(tGuiClock1 *pClient);
void ui_Clock1_set_time(tGuiClock1 *pClient, cmnDateTime_t time);

#ifdef __cplusplus
}
#endif



#endif