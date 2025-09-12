#!/bin/bash
################################################################
# Get current script directory
################################################################
if [[ -n "$PRJ_TOP" ]]; then
    echo "PRJ_TOP was set to ${PRJ_TOP}."
else
    echo "Have you sourced the setup.env already?"
    exit 1
fi

rebuild() {
    rm -rf ${PRJ_TOP}/build && mkdir ${PRJ_TOP}/build && cd ${PRJ_TOP}/build
    cmake $1 .. && make -j32
    cd ${PRJ_TOP}
}


info() {
    ${METOPE_ARM_TOOLCHAIN_REPO}/bin/arm-none-eabi-objdump -h "${PRJ_TOP}/build/model1.elf" > ${PRJ_TOP}/objdump.log 2>&1
}


source setup.env && rebuild "-DCMAKE_BUILD_TYPE=Debug -DUSE_REGISTER=1 -DLOG_LEVEL=2" > ${PRJ_TOP}/build.log 2>&1
if [ ! -e "${PRJ_TOP}/build/model1.elf" ]; then
    echo ".elf file was NOT found!"
    exit 1
fi
info
sh pause.sh
sh clean.sh

source setup.env && rebuild "-DCMAKE_BUILD_TYPE=Debug -DTEST_ONLY=1 -DUSE_REGISTER=1 -DINCLUDE_TB_CMN=1" > ${PRJ_TOP}/build.log 2>&1
if [ ! -e "${PRJ_TOP}/build/model1.elf" ]; then
    echo ".elf file was NOT found!"
    exit 1
fi

qemu-system-arm -M stm32-f411ce -m 1024 -smp 4 -cpu cortex-m4 -kernel ${PRJ_TOP}/build/model1.elf

sh pause.sh
sh clean.sh

