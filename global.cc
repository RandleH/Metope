


#include "global.h"
#include "cmn_device.h"


#ifdef __cplusplus
extern "C"{
#endif

tMainSystemStatus  metope = {
  .bsp = {
    .screen = {
      ._brightness = BSP_SCREEN_DEFAULT_BRIGHTNESS,
    },
    .led = {
      ._port = &LED_GPIO_Port->ODR
    },
  },
  .dev = {
    .pHspi2  = &hspi2,
    .pHuart2 = &huart2,
    .pHtim3  = &htim3
  },
  .app = {
    .lvgl = {
#if LVGL_VERSION==836
      .disp_drv      = {0},
      .disp          = NULL,
      .disp_draw_buf = {0},
      .gram          = {0},
      .isFlushDone   = true
#elif LVGL_VERSION==922
      .pDisplayHandle = NULL,
      .pLvglTheme     = NULL
#endif
    }
  }
};

#ifdef __cplusplus
}
#endif


#if (defined TEST_ONLY) && (TEST_ONLY==1) && (defined __cplusplus)

LocalProjectTest     tb_infra_local;
HumanInteractionTest tb_infra_hmi;

#endif
