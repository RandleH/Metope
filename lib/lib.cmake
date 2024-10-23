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
set( CPU_FLAG_STM32 "")
set( LINK_FLAG_STM32 "")

if((NOT CHIP) OR (CHIP STREQUAL "STM32F411CEU6"))
    list(APPEND DEF_STM32       "-DSTM32F411xE"
                                "-DUSE_HAL_DRIVER" )
    aux_source_directory( ${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src  SRC_STM32)
    list(APPEND SRC_LIST        "${PRJ_TOP}/lib/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f411xe.s"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c" )
    
    list(APPEND INC_STM32       "${PRJ_TOP}/lib/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include/"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/CMSIS/Core/Include/"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc" )
    
    list(APPEND LINK_FLAG_STM32 "-T${PRJ_TOP}/STM32F411CEUX_FLASH.ld")

    list(APPEND CPU_FLAG_STM32  "-mcpu=cortex-m4")

elseif((CHIP STREQUAL "STM32F405RGT6"))
    list(APPEND DEF_STM32       "-DSTM32F405xx"
                                "-DUSE_HAL_DRIVER" )
    aux_source_directory( ${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src  SRC_STM32)
    list(APPEND SRC_STM32       "${PRJ_TOP}/lib/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f405xx.s"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c" )

    list(APPEND INC_STM32       "${PRJ_TOP}/lib/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include/"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/CMSIS/Core/Include/"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc" )
    
    list(APPEND LINK_FLAG_STM32 "-T${PRJ_TOP}/STM32F405RGTx_FLASH.ld")
    
    list(APPEND CPU_FLAG_STM32  "-mcpu=cortex-m4")
else()
    message(FATAL_ERROR "Unknown system target")
endif()





list( APPEND        DEF_LIST    ${DEF_STM32})
list( APPEND        SRC_LIST    ${SRC_STM32})
list( REMOVE_ITEM   SRC_LIST    "${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_timebase_tim_template.c"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_msp_template.c"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_timebase_rtc_alarm_template.c"
                                "${PRJ_TOP}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_timebase_rtc_wakeup_template.c" )

list( APPEND        INC_DIR_LIST ${INC_STM32})
list( APPEND        LINK_FLAG    ${LINK_FLAG_STM32})
list( APPEND        CPU_FLAG     ${CPU_FLAG_STM32})