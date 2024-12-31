set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(TOOLCHAIN_PREFIX arm-none-eabi-)





if(${CMAKE_HOST_APPLE})
    if(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "arm64")
        set(ARM_TOOLCHAIN_REPO "macOS-AppleSilicon")
    else()
        set(ARM_TOOLCHAIN_REPO "macOS-Intel")
    endif()
elseif(${CMAKE_HOST_UNIX})
    message(FATAL_ERROR "Currently unsupport UNIX/LINUX host system")
elseif(${CMAKE_HOST_WIN32})
    message(FATAL_ERROR "Currently unsupport WIN32 host system")
endif()

set(ARM_TOOLCHAIN_DIR  ${PRJ_TOP}/lib/arm-toolchain/${ARM_TOOLCHAIN_REPO})

message(STATUS "Arm-GNU-Tool-Chain Repository was set to ${ARM_TOOLCHAIN_REPO}")
message(STATUS "Arm-GNU-Tool-Chain Directory was set to ${ARM_TOOLCHAIN_DIR}")


find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Git Submodule Update: Arm-GNU-Tool-Chain")
        execute_process(COMMAND           ${GIT_EXECUTABLE} submodule update --init ${ARM_TOOLCHAIN_REPO}
                        WORKING_DIRECTORY ${PRJ_TOP}/lib/arm-toolchain
                        RESULT_VARIABLE   GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()


set(BINUTILS_PATH ${ARM_TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc)

if (NOT BINUTILS_PATH)
    message(FATAL_ERROR "ARM GCC toolchain not found")
endif ()

# Without that flag CMake is not able to pass test compilation check
if (${CMAKE_VERSION} VERSION_EQUAL "3.6.0" OR ${CMAKE_VERSION} VERSION_GREATER "3.6")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
else ()
    set(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs")
endif ()

set(CMAKE_C_COMPILER   ${ARM_TOOLCHAIN_DIR}/bin/${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${ARM_TOOLCHAIN_DIR}/bin/${TOOLCHAIN_PREFIX}g++)
set(CMAKE_AR           ${ARM_TOOLCHAIN_DIR}/bin/${TOOLCHAIN_PREFIX}gcc-ar)
set(CMAKE_RANLIB       ${ARM_TOOLCHAIN_DIR}/bin/${TOOLCHAIN_PREFIX}gcc-ranlib)

execute_process(COMMAND ${CMAKE_C_COMPILER} -print-sysroot
    OUTPUT_VARIABLE ARM_GCC_SYSROOT OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_OBJCOPY ${ARM_TOOLCHAIN_DIR}/bin/${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "objcopy tool")
set(CMAKE_SIZE_UTIL ${ARM_TOOLCHAIN_DIR}/bin/${TOOLCHAIN_PREFIX}size CACHE INTERNAL "size tool")

set(CMAKE_SYSROOT ${ARM_GCC_SYSROOT})
set(CMAKE_FIND_ROOT_PATH ${BINUTILS_PATH}; ${ARM_TOOLCHAIN_DIR}/bin)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
