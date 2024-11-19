


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
    },
    .rtos = {.status = OFF},
    .clock = {0}
  },
  .info = {
    .system_initial_time = {
      /**
       * @name    Time Stamp  __TIME_STAMP__
       * @example Thu Oct 19 22:41:50 2023
       * @note    [0:2]   Weekday
       *          [4:6]   Month
       *          [8:9]   Date
       *          [11:12] Hour
       *          [14:15] Minute
       *          [17:18] Second
       *          [20:23] Year
      */
      {
#if 0 // No idea why this doesn't work
        .year   = (__TIMESTAMP__[20]-'0')*1000 + (__TIMESTAMP__[21]-'0')*100 + (__TIMESTAMP__[22]-'0')*10 + (__TIMESTAMP__[23]-'0') - 2024,
        .month  = 11, // @todo: Fix the correct month
        .day    = (__TIMESTAMP__[8]-'0')*10 + (__TIMESTAMP__[9]-'0'),
        .hour   = (__TIMESTAMP__[11]-'0')*10 + (__TIMESTAMP__[12]-'0'),
        .minute = (__TIMESTAMP__[14]-'0')*10 + (__TIMESTAMP__[15]-'0'),
        .second = (__TIMESTAMP__[17]-'0')*10 + (__TIMESTAMP__[18]-'0')
#else
        .year   = 0,
        .month  = 11,
        .day    = 19,
        .hour   = 11,
        .minute = 54,
        .second = 0
#endif
      }
    },
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
