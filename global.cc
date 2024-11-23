


#include "global.h"
#include "cmn_device.h"


#ifdef __cplusplus
extern "C"{
#endif

tMainSystemStatus  metope = {
  .bsp = {
    .screen = {
      .brightness = BSP_SCREEN_DEFAULT_BRIGHTNESS,
      .refresh_rate_ms = 10
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
    },
    .rtos = {.status = OFF},
    .clock = {
      .time = {
        {
          .year   = 0,
          .month  = 11,
          .day    = 22,
          .hour   = 19,
          .minute = 2,
          .second = 0
        }
      },
      ._handle = NULL,
      .style = kAppGuiClock_LVVVW,
      .gui = {0},
    }
  },
  .info = {
    .status = {
      {
        .display_off = 0,
        .reserved    = 0
      }
    }
  }
};

volatile u32 gDummy = 0;

#ifdef __cplusplus
}
#endif


#if (defined TEST_ONLY) && (TEST_ONLY==1) && (defined __cplusplus)

#if (defined INCLUDE_TB_CMN) && (INCLUDE_TB_CMN==1)
LocalProjectTest     tb_infra_local;
#endif

#if (defined INCLUDE_TB_HMI) && (INCLUDE_TB_HMI==1)
HumanInteractionTest tb_infra_hmi;
#endif

#if (defined INCLUDE_TB_OS) && (INCLUDE_TB_OS==1)
Test                 tb_infra_os;
#endif

#endif
