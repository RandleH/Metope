file(GLOB_RECURSE SRC_DIR__BSP CONFIGURE_DEPENDS    "${PRJ_TOP}/bsp/*.h" 
                                                    "${PRJ_TOP}/bsp/*.cc" 
                                                    "${PRJ_TOP}/bsp/*.c" )


list( APPEND SRC_LIST ${SRC_DIR__BSP})

if($ENV{METOPE_CHIP} STREQUAL "NATIVE")
    list( APPEND SRC_LIST_TO_BE_REMOVED     "${PRJ_TOP}/bsp/bsp_battery.c"
                                            "${PRJ_TOP}/bsp/bsp_cpu.c"
                                            "${PRJ_TOP}/bsp/bsp_gyro.c"
                                            "${PRJ_TOP}/bsp/bsp_led.c"
                                            "${PRJ_TOP}/bsp/bsp_rtc.c"
                                            "${PRJ_TOP}/bsp/bsp_screen.c"
                                            "${PRJ_TOP}/bsp/bsp_timer.c"
                                            "${PRJ_TOP}/bsp/bsp_uart.c" )

    message( STATUS "Remove the bsp source files: ${SRC_LIST_TO_BE_REMOVED}")
    list( REMOVE_ITEM SRC_LIST ${SRC_LIST_TO_BE_REMOVED})
endif()

set( INC_DIR__BSP "")
GET_SUBDIR( INC_DIR__BSP ${PRJ_TOP}/bsp)

list(APPEND INC_LIST ${PRJ_TOP}/bsp)
list(APPEND INC_LIST ${INC_DIR__BSP})

if($ENV{METOPE_CHIP} STREQUAL "NATIVE")
    list( APPEND INC_LIST_TO_BE_REMOVED     "${PRJ_TOP}/bsp/include/bsp_battery.h"
                                            "${PRJ_TOP}/bsp/include/bsp_cpu.h"
                                            "${PRJ_TOP}/bsp/include/bsp_gyro.h"
                                            "${PRJ_TOP}/bsp/include/bsp_led.h"
                                            "${PRJ_TOP}/bsp/include/bsp_rtc.h"
                                            "${PRJ_TOP}/bsp/include/bsp_screen.h"
                                            "${PRJ_TOP}/bsp/include/bsp_timer.h"
                                            "${PRJ_TOP}/bsp/include/bsp_uart.h" )

    message( STATUS "Remove the bsp header files: ${INC_LIST_TO_BE_REMOVED}")
    list( REMOVE_ITEM INC_LIST ${INC_LIST_TO_BE_REMOVED})
endif()