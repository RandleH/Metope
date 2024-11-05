#ifdef LVGL_VERSION
  #if LVGL_VERSION==922
    #include "./lv_conf_9_2_2.h"
  #elif LVGL_VERSION==836
    #include "./lv_conf_8_3_6.h"
  #endif
#else
  #error "Please specify the LVGL version. See details in CMakeLists.txt."
#endif