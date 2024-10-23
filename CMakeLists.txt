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
list(APPEND SRC_LIST                "${PRJ_TOP}/main.c" 
                                    "${PRJ_TOP}/init.c")

#########################################################################################################
# Target Compile Option Flag
#########################################################################################################
set( CXX_FLAG                       "-fdata-sections"
                                    "-ffunction-sections"
                                    "-Wall"
                                    "-Wno-pedantic")

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
                                    "-Wl,--gc-sections"
                                    )



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
elseif (CHIP STREQUAL "STM32F405RGT6")
    set(CHIP "STM32F405RGT6" CACHE INTERNAL "")
else()
    message(FATAL_ERROR "Unknown system target")
endif()
include( ${PRJ_TOP}/cmn/cmn.cmake)
include( ${PRJ_TOP}/lib/lib.cmake)


add_compile_options( ${CPU_FLAG})
add_link_options( ${CPU_FLAG})

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
                        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")



