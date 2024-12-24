#########################################################################################################
# Library: STM32 HAL
# Website: https://www.st.com
# GitHub: https://github.com/STMicroelectronics/STM32CubeF4.git
#########################################################################################################

# CHECK_LIB_INTEGRITY( "${PRJ_TOP}/lib/STM32CubeF4" result)
# if( ${result} STREQUAL FALSE)
#     FetchContent_Declare(   stm32hal
#                             GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeF4.git
#                             GIT_TAG        master
#                             SOURCE_DIR "${PRJ_TOP}/lib/STM32CubeF4")
#     FetchContent_MakeAvailable(stm32hal)
# endif()

set( INC_STM32 "")
set( SRC_STM32 "")
set( DEF_STM32 "")
set( LINK_FLAG_STM32 "")

if((NOT CHIP) OR (CHIP STREQUAL "STM32F411CEU6"))
    list(APPEND DEF_STM32       "-DSTM32F411xE"
                                "-DUSE_HAL_DRIVER" )

    aux_source_directory( "${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src"  SRC_STM32)
    list(APPEND SRC_STM32       "${PRJ_TOP}/STM32CubeMX/STM32F411CEU6/startup_stm32f411xe.s"
                                "${PRJ_TOP}/STM32CubeMX/STM32F411CEU6/Core/Src/system_stm32f4xx.c" 
                                )


    list(APPEND INC_STM32       "${PRJ_TOP}/lib/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include/"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/CMSIS/Core/Include/"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc" 
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/CMSIS/DSP/Include" 
                                "${PRJ_TOP}/STM32CubeMX/STM32F411CEU6/Core/Inc/"
                                )
                                
    list(APPEND LINK_FLAG_STM32 "-T${PRJ_TOP}/STM32F411CEUX_FLASH.ld")

elseif((CHIP STREQUAL "STM32F405RGT6"))
    list(APPEND DEF_STM32       "-DSTM32F405xx"
                                "-DUSE_HAL_DRIVER" )
    aux_source_directory( ${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src  SRC_STM32)
    list(APPEND SRC_STM32       "${PRJ_TOP}/STM32CubeMX/STM32F405RGT6/startup_stm32f405xx.s"
                                "${PRJ_TOP}/STM32CubeMX/STM32F405RGT6/Core/Src/system_stm32f4xx.c" )

    list(APPEND INC_STM32       "${PRJ_TOP}/lib/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include/"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/CMSIS/Core/Include/"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc" 
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/CMSIS/DSP/Include" )
                                
    list(APPEND INC_STM32       "${PRJ_TOP}/STM32CubeMX/STM32F405RGT6/Core/Inc/")
    
    list(APPEND LINK_FLAG_STM32 "-T${PRJ_TOP}/STM32F405RGTx_FLASH.ld")
else()
    message(FATAL_ERROR "Unknown system target")
endif()


set( INC_FREERTOS "")
set( SRC_FREERTOS "")
set( DEF_FREERTOS "")

# aux_source_directory( "${PRJ_TOP}/lib/STM32CubeF4/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2" SRC_FREERTOS)

list(APPEND SRC_FREERTOS        "${PRJ_TOP}/lib/STM32CubeF4/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c"
                                "${PRJ_TOP}/lib/STM32CubeF4/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c"
                                "${PRJ_TOP}/lib/STM32CubeF4/Middlewares/Third_Party/FreeRTOS/Source/croutine.c"
                                "${PRJ_TOP}/lib/STM32CubeF4/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c"
                                "${PRJ_TOP}/lib/STM32CubeF4/Middlewares/Third_Party/FreeRTOS/Source/list.c"
                                "${PRJ_TOP}/lib/STM32CubeF4/Middlewares/Third_Party/FreeRTOS/Source/queue.c"
                                "${PRJ_TOP}/lib/STM32CubeF4/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"
                                "${PRJ_TOP}/lib/STM32CubeF4/Middlewares/Third_Party/FreeRTOS/Source/tasks.c"
                                "${PRJ_TOP}/lib/STM32CubeF4/Middlewares/Third_Party/FreeRTOS/Source/timers.c" )


list(APPEND INC_FREERTOS        "${PRJ_TOP}/lib/STM32CubeF4/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2"
                                "${PRJ_TOP}/lib/STM32CubeF4/Middlewares/Third_Party/FreeRTOS/Source/include"
                                "${PRJ_TOP}/lib/STM32CubeF4/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" )

list(APPEND DEF_FREERTOS        "-DCMSIS_device_header=\"${PRJ_TOP}/device.h\"")


list( APPEND        DEF_LIST    "${DEF_STM32}" "${DEF_FREERTOS}")
list( APPEND        SRC_LIST    "${SRC_STM32}" "${SRC_FREERTOS}")
list( REMOVE_ITEM   SRC_LIST    "${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_timebase_tim_template.c"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_msp_template.c"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_timebase_rtc_alarm_template.c"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_timebase_rtc_wakeup_template.c" )

list( APPEND        INC_DIR_LIST "${INC_STM32}" "${PRJ_TOP}/lib" "${INC_FREERTOS}")

list( APPEND        LINK_FLAG    ${LINK_FLAG_STM32})






message( STATUS "Check library integrity @ ${PRJ_TOP}/lib/lvgl ..." )


set( LV_CONF_BUILD_DISABLE_EXAMPLES  TRUE)
set( DLV_CONF_BUILD_DISABLE_DEMOS    TRUE)
set( LVGL_MISC_DEFINE       "-DLV_CONF_BUILD_DISABLE_EXAMPLES=1"
                            "-DLV_CONF_BUILD_DISABLE_DEMOS=1"
                            "-DLV_CONF_PATH=${PRJ_TOP}/lib/lv_conf.h"
                            )
if( LVGL_VERSION_9_2_2 )
    list(APPEND LVGL_MISC_DEFINE "-DLVGL_VERSION=922")
else()
    list(APPEND LVGL_MISC_DEFINE "-DLVGL_VERSION=836")
endif()

add_definitions( ${LVGL_MISC_DEFINE})
add_subdirectory( ${PRJ_TOP}/lib/lvgl ${PRJ_TOP}/build/lvgl/build )


target_include_directories(lvgl PUBLIC ${INC_DIR_LIST})
target_compile_options(lvgl  INTERFACE   ${CMAKE_CXX_FLAGS} ${CPU_FLAG} ${CXX_FLAG} )
target_link_options(lvgl INTERFACE ${LINK_FLAG} ${CPU_FLAG})

# lvgl_demos
# lvgl_example


list( APPEND INC_DIR_LIST   ${PRJ_TOP}/lib/lvgl)
list( APPEND TARGET_LIST    lvgl)

message( STATUS "LVGL library loaded")

