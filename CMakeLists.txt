# Ref: https://www.reddit.com/r/embedded/comments/qkcfjt/cmake_setup_for_project_with_stm32_how_to_switch/
# Ref: https://gist.github.com/eugene-babichenko/3118042a23d2082bfba624f3f2039843
# Ref: https://stackoverflow.com/questions/36122358/visual-studio-code-c-include-path

cmake_minimum_required(VERSION 3.19.2)

set(CMAKE_MESSAGE_CONTEXT_SHOW true)

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


if ($ENV{METOPE_EMULATOR} EQUAL 1)
    if( UNIT_TEST AND UNIT_TEST EQUAL 1)
        message(STATUS "File operation: $ENV{PRJ_TOP}/backup/main_unittest.cc => $ENV{PRJ_TOP}/main.cc")
        file(COPY_FILE "$ENV{PRJ_TOP}/backup/main_unittest.cc" "$ENV{PRJ_TOP}/main.cc")
    else()
        message(STATUS "File operation: $ENV{PRJ_TOP}/backup/main_emulator.cc => $ENV{PRJ_TOP}/main.cc")
        file(COPY_FILE "$ENV{PRJ_TOP}/backup/main_emulator.cc" "$ENV{PRJ_TOP}/main.cc")
    endif()
else()
    if ($ENV{METOPE_CHIP} STREQUAL "NATIVE")
        message(STATUS "File operation: $ENV{PRJ_TOP}/backup/main_native.cc => $ENV{PRJ_TOP}/main.cc")
        file(COPY_FILE "$ENV{PRJ_TOP}/backup/main_native.cc" "$ENV{PRJ_TOP}/main.cc")
    else()
        message(STATUS "File operation: $ENV{PRJ_TOP}/backup/main_program.cc => $ENV{PRJ_TOP}/main.cc")
        file(COPY_FILE "$ENV{PRJ_TOP}/backup/main_program.cc" "$ENV{PRJ_TOP}/main.cc")
    endif()
endif()



#########################################################################################################
# Project Path & Source Files Definition
#########################################################################################################
set( PRJ_TOP                        ${CMAKE_SOURCE_DIR})
set( DEF_LIST                       "")
set( SRC_LIST                       "")
set( INC_LIST                       "")
set( TARGET_LIST                    "")


message( STATUS "Project Directory        => ${PRJ_TOP}")
message( STATUS "System Processor         => ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message( STATUS "System Name              => ${CMAKE_HOST_SYSTEM_NAME}")
message( STATUS "CMake System             => ${CMAKE_SYSTEM}")
message( STATUS "CMake System Version     => ${CMAKE_SYSTEM_VERSION}")
message( STATUS "CMake System Arch        => ${CMAKE_OSX_ARCHITECTURES}")
message( STATUS "CMake Build Type         => ${CMAKE_BUILD_TYPE}")
message( STATUS "APPLE | WIN32 | UNIX     => ${APPLE} | ${WIN32} | ${UNIX}") # True if running on Mac OSX. | True on windows systems, including win64. | True for UNIX and UNIX like operating systems.
message( STATUS "Apple Silicon            => ${CMAKE_APPLE_SILICON_PROCESSOR}")
message( STATUS "Host Unix                => ${CMAKE_HOST_UNIX}")  # True for UNIX and UNIX like operating systems.
message( STATUS "Host Windows             => ${CMAKE_HOST_WIN32}") # True on windows systems, including win64.
message( STATUS "Host Apple               => ${CMAKE_HOST_APPLE}") # True for Apple OSXoperating systems.



# Tool Chain Setup
if (NOT $ENV{METOPE_CHIP} STREQUAL "NATIVE")
    set(CMAKE_CROSSCOMPILING ON CACHE BOOL "Cross-compiling" FORCE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/arm-gcc-toolchain.cmake)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS       ON)


# Project Setup
project( model1 C CXX ASM)

set(CMAKE_C_STANDARD                    99)
set(CMAKE_C_STANDARD_REQUIRED           ON)
set(CMAKE_C_EXTENSIONS                  ON)
set(CMAKE_CXX_STANDARD                  17)



# Default C compiler flags
set(CMAKE_C_FLAGS_DEBUG_INIT "-g3 -Og -Wall -pedantic -DDEBUG")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG_INIT}" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -Wall")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE_INIT}" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_MINSIZEREL_INIT "-Os -Wall")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL_INIT}" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O2 -g -Wall")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO_INIT}" CACHE STRING "" FORCE)
# Default C++ compiler flags
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g3 -Og -Wall -pedantic -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG_INIT}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-Os -Wall")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL_INIT}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g -Wall")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT}" CACHE STRING "" FORCE)


#########################################################################################################
# Target Definition
#########################################################################################################
list(APPEND INC_LIST                "${PRJ_TOP}")
list(APPEND SRC_LIST                "${PRJ_TOP}/main.cc")



set( CXX_FLAG                       "-Wall"
                                    "-Wno-pedantic"
                                    "-Wno-gnu-zero-variadic-macro-arguments")

set( CPU_FLAG                       "")

set( LINK_FLAG                      "")

if (NOT $ENV{METOPE_CHIP} STREQUAL "NATIVE")
    #########################################################################################################
    # Target Compile Option Flag
    #########################################################################################################
    list(APPEND CXX_FLAG                "-fdata-sections"
                                        "-ffunction-sections"
                                        "-fstack-usage")

    list(APPEND CPU_FLAG                "-mthumb"
                                        "-mfloat-abi=hard"
                                        "-mfpu=fpv4-sp-d16"
                                        "-mcpu=cortex-m4")
    #########################################################################################################
    # Target Linking Option Flag
    #########################################################################################################
    list(APPEND LINK_FLAG               "-lc"
                                        "-lm"
                                        "-lnosys"
                                        "-Wl,-Map=${PROJECT_NAME}.map,--cref"
                                        "-Wl,--gc-sections,--print-memory-usage,--print-output-format")
    
    if($ENV{METOPE_EMULATOR} EQUAL 1)
        list(APPEND LINK_FLAG "-specs=rdimon.specs")
    else()
        list(APPEND LINK_FLAG "-specs=nosys.specs")
    endif()
else()
    # Native Mode
    list(APPEND CXX_FLAG                "-Wno-gnu-anonymous-struct"
                                        "-Wno-gnu-case-range")
endif()


add_compile_options( ${CPU_FLAG} ${CXX_FLAG})
add_link_options( ${LINK_FLAG})


if((NOT IMU) OR (IMU STREQUAL "QMI8658A") )
    list(APPEND DEF_LIST "-DQMI8658A=1")
elseif(IMU STREQUAL "QMI8658C")
    list(APPEND DEF_LIST "-DQMI8658C=1")
elseif(IMU STREQUAL "MPU6050")
    list(APPEND DEF_LIST "-DMPU6050=1")
endif()



#########################################################################################################
# Additional Setting for Debug or Release
#########################################################################################################
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message( STATUS "[CMakeLists] Target was set to debug.")
    include( ${PRJ_TOP}/cmake/debug.cmake)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message( STATUS "[CMakeLists] Target was set to release.")
    include( ${PRJ_TOP}/cmake/release.cmake)
endif()


#########################################################################################################
# Add Modules
#########################################################################################################
message( STATUS "[CMakeLists] Deploying project modules...")

if( DEFINED ENV{METOPE_CHIP})
    if($ENV{METOPE_EMULATOR} EQUAL 1)
        message(STATUS "[CMakeLists] Added EMULATOR_$ENV{METOPE_CHIP} to the definition list.")
        list(APPEND DEF_LIST "-DEMULATOR_$ENV{METOPE_CHIP}")
    else()
        message(STATUS "[CMakeLists] Added SYS_TARGET_$ENV{METOPE_CHIP} to the definition list.")
        list(APPEND DEF_LIST "-DSYS_TARGET_$ENV{METOPE_CHIP}")
    endif()
else()
    message( FATAL_ERROR "METOPE_CHIP was NOT defined. Have you sourced the setup.env?")
endif()


#########################################################################################################
# Customized Project Macros
# @param UNIT_TEST      - Test Bench Program was enabled. Escape the main program.
#   @param INCLUDE_TB_BSP - Enable Human Interaction Test Bench (ie. Tests that need manual intervine)
#   @param INCLUDE_TB_CMN - Enable Common Test Bench (ie. Basic utility functional tests)
#   @param INCLUDE_TB_OS  - Enable OS Test Bench (ie. RTOS related Test Bench)
#########################################################################################################
if( UNIT_TEST AND UNIT_TEST EQUAL 1)
    list(APPEND DEF_LIST "-DUNIT_TEST=1")
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

include( ${PRJ_TOP}/cmn/cmn.cmake)
include( ${PRJ_TOP}/app/app.cmake)
include( ${PRJ_TOP}/bsp/bsp.cmake)
include( ${PRJ_TOP}/top/top.cmake)
if (NOT $ENV{METOPE_CHIP} STREQUAL "NATIVE")
    include( ${PRJ_TOP}/lib/lib.cmake)
endif()


#########################################################################################################
# Final Execution 
#########################################################################################################
message( STATUS "Deploying targets...")
add_definitions( ${DEF_LIST})
include_directories( BEFORE ${INC_LIST})

add_executable( ${PROJECT_NAME}.elf  ${SRC_LIST})


#########################################################################################################
# Output BIN/HEX files
#########################################################################################################
if (NOT $ENV{METOPE_CHIP} STREQUAL "NATIVE")
    target_compile_options ( ${PROJECT_NAME}.elf  PUBLIC      ${CMAKE_CXX_FLAGS} ${CPU_FLAG} ${CXX_FLAG})
    target_link_options    ( ${PROJECT_NAME}.elf  PUBLIC      ${LINK_FLAG} ${CPU_FLAG} )
    target_link_libraries( ${PROJECT_NAME}.elf PUBLIC  ${TARGET_LIST})

    set(HEX_FILE ${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
    set(BIN_FILE ${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}.bin)
    add_custom_command(     TARGET  ${PROJECT_NAME}.elf POST_BUILD
                            COMMAND ${CMAKE_OBJCOPY} -Oihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
                            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
                            COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}"
                            COMMAND "${ARM_TOOLCHAIN_DIR}/arm-none-eabi/bin/nm"      "--print-size" "--size-sort" "${PROJECT_NAME}.elf" ">" "${PRJ_TOP}/build/${PROJECT_NAME}.nm"
                            COMMAND "${ARM_TOOLCHAIN_DIR}/arm-none-eabi/bin/objdump" "-d" "${PROJECT_NAME}.elf"  ">" "${PRJ_TOP}/build/${PROJECT_NAME}.dis" 
                            COMMAND "${ARM_TOOLCHAIN_DIR}/bin/arm-none-eabi-size"    "${PRJ_TOP}/build/model1.elf"
                        )
endif()