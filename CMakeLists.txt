# Ref: https://www.reddit.com/r/embedded/comments/qkcfjt/cmake_setup_for_project_with_stm32_how_to_switch/
# Ref: https://gist.github.com/eugene-babichenko/3118042a23d2082bfba624f3f2039843
# Ref: https://stackoverflow.com/questions/36122358/visual-studio-code-c-include-path

cmake_minimum_required(VERSION 3.13)



MACRO( GET_SUBDIR result curdir)
    file(GLOB_RECURSE children LIST_DIRECTORIES TRUE CONFIGURE_DEPENDS "${curdir}/*" )
    SET(dirlist "")
    FOREACH(child ${children})
        IF(IS_DIRECTORY ${child})
            LIST(APPEND dirlist ${child})
        ENDIF()
    ENDFOREACH()
    SET(${result} ${dirlist})
ENDMACRO()


# Tool Chain Setup
set(CMAKE_TOOLCHAIN_FILE                ${CMAKE_SOURCE_DIR}/cmake/arm-gcc-toolchain.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS       ON)

# Project Setup
project( model1 C CXX ASM)

set(CMAKE_C_STANDARD                    99)
set(CMAKE_C_STANDARD_REQUIRED           ON)
set(CMAKE_C_EXTENSIONS                  ON)
set(CMAKE_CXX_STANDARD                  17)
set(CMAKE_CROSSCOMPILING ON CACHE BOOL "Cross-compiling" FORCE)


#########################################################################################################
# Project Path & Source Files Definition
#########################################################################################################
set( PRJ_TOP                        ${CMAKE_SOURCE_DIR})
set( DEF_LIST                       "")
set( SRC_LIST                       "")
set( INC_DIR_LIST                   "")
set( TARGET_LIST                    "")


#########################################################################################################
# Target Definition
#########################################################################################################

list(APPEND INC_DIR_LIST            "${PRJ_TOP}")
list(APPEND SRC_LIST                "${PRJ_TOP}/main.cc" 
                                    "${PRJ_TOP}/init.c"
                                    "${PRJ_TOP}/global.cc")

#########################################################################################################
# Target Compile Option Flag
#########################################################################################################
set( CXX_FLAG                       "-fdata-sections"
                                    "-ffunction-sections"
                                    "-Wall"
                                    "-Wno-pedantic"
                                    "-fstack-usage")

set( CPU_FLAG                       "-mthumb"
                                    "-mfloat-abi=hard"
                                    "-mfpu=fpv4-sp-d16")


#########################################################################################################
# Target Linking Option Flag
#########################################################################################################
set( LINK_FLAG                      "-specs=nosys.specs"
                                    "-lc"
                                    "-lm"
                                    "-lnosys"
                                    "-Wl,-Map=${PROJECT_NAME}.map,--cref"
                                    "-Wl,--gc-sections,--print-memory-usage,--print-output-format")

if((NOT CHIP) OR (CHIP STREQUAL "STM32F411CEU6") OR (CHIP STREQUAL "STM32F405RGT6"))
    list(APPEND CPU_FLAG  "-mcpu=cortex-m4")
endif()

if((NOT IMU) OR (IMU STREQUAL "QMI8658A") )
    list(APPEND DEF_LIST "-DQMI8658A=1")
elseif(IMU STREQUAL "QMI8658C")
    list(APPEND DEF_LIST "-DQMI8658C=1")
elseif(IMU STREQUAL "MPU6050")
    list(APPEND DEF_LIST "-DMPU6050=1")
endif()

add_compile_options( ${CPU_FLAG})
add_link_options( ${CPU_FLAG})


#########################################################################################################
# Additional Setting for Debug or Release
#########################################################################################################
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message( STATUS "Target set to debug version.")
    include( ${PRJ_TOP}/cmake/debug.cmake)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message( STATUS "Target set to release.")
    include( ${PRJ_TOP}/cmake/release.cmake)
endif()



#########################################################################################################
# Add Modules
#########################################################################################################
message( STATUS "Deploying project modules...")
if((NOT CHIP) OR (CHIP STREQUAL "STM32F411CEU6"))
    set(CHIP "STM32F411CEU6" CACHE INTERNAL "")
    list(APPEND DEF_LIST "-DSYS_TARGET_STM32F411CEU6")
elseif (CHIP STREQUAL "STM32F405RGT6")
    set(CHIP "STM32F405RGT6" CACHE INTERNAL "")
    list(APPEND DEF_LIST "-DSYS_TARGET_STM32F405RGT6")
else()
    message(FATAL_ERROR "Unknown system target")
endif()

#########################################################################################################
# Customized Project Macros
# @param TEST_ONLY      - Test Bench Program was enabled. Escape the main program.
#   @param INCLUDE_TB_BSP - Enable Human Interaction Test Bench (ie. Tests that need manual intervine)
#   @param INCLUDE_TB_CMN - Enable Common Test Bench (ie. Basic utility functional tests)
#   @param INCLUDE_TB_OS  - Enable OS Test Bench (ie. RTOS related Test Bench)
#
# @param USE_REGISTER   - Bypass the HAL library support. Use bare register instruction. (Partially Supported)

#########################################################################################################
if( TEST_ONLY AND TEST_ONLY EQUAL 1)
    list(APPEND DEF_LIST "-DTEST_ONLY=1")
    if(INCLUDE_TB_BSP AND INCLUDE_TB_BSP EQUAL 1)
        list(APPEND DEF_LIST "-DINCLUDE_TB_BSP=1")
    endif()
    if(INCLUDE_TB_CMN AND INCLUDE_TB_CMN EQUAL 1)
        list(APPEND DEF_LIST "-DINCLUDE_TB_CMN=1")
    endif()
    if(INCLUDE_TB_OS AND INCLUDE_TB_OS EQUAL 1)
        list(APPEND DEF_LIST "-DINCLUDE_TB_OS=1")
    endif()
    include( ${PRJ_TOP}/test/test.cmake)
endif()

if( USE_REGISTER AND USE_REGISTER EQUAL 1)
    list(APPEND DEF_LIST "-DUSE_REGISTER=1")
endif()


include( ${PRJ_TOP}/cmn/cmn.cmake)
include( ${PRJ_TOP}/app/app.cmake)
include( ${PRJ_TOP}/bsp/bsp.cmake)
include( ${PRJ_TOP}/lib/lib.cmake)

if( CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(${PRJ_TOP}/cmake/debug.cmake)
else()
    include(${PRJ_TOP}/cmake/released.cmake)
endif()


#########################################################################################################
# Final Execution 
#########################################################################################################
message( STATUS "Deploying targets...")
add_definitions( ${DEF_LIST})
include_directories( BEFORE ${INC_DIR_LIST})

add_executable( ${PROJECT_NAME}.elf  ${SRC_LIST})

target_compile_options ( ${PROJECT_NAME}.elf  PUBLIC      ${CMAKE_CXX_FLAGS} ${CPU_FLAG} ${CXX_FLAG})
target_link_options    ( ${PROJECT_NAME}.elf  PUBLIC      ${LINK_FLAG} ${CPU_FLAG} )
target_link_libraries( ${PROJECT_NAME}.elf PUBLIC  ${TARGET_LIST})

message("================================================================================")
message( "Executable target property: ${PROJECT_NAME}.elf")
message("================================================================================")
# print_target_properties( ${PROJECT_NAME}.elf)


#########################################################################################################
# Output BIN/HEX files
#########################################################################################################
set(HEX_FILE ${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(BIN_FILE ${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}.bin)
add_custom_command(     TARGET  ${PROJECT_NAME}.elf POST_BUILD
                        COMMAND ${CMAKE_OBJCOPY} -Oihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
                        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
                        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}"
                        COMMAND "nm" "--print-size" "--size-sort" "${PROJECT_NAME}.elf" ">" "${PRJ_TOP}/build/${PROJECT_NAME}.nm"
                        COMMAND "objdump" "-d" "${PROJECT_NAME}.elf"  ">" "${PRJ_TOP}/build/${PROJECT_NAME}.dis" )
