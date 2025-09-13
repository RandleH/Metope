#!/bin/bash
################################################################
# Get current script directory
################################################################


rebuild() {
    rm -rf ${PRJ_TOP}/build && mkdir ${PRJ_TOP}/build && cd ${PRJ_TOP}/build
    cmake $1 .. && make -j32
    cd ${PRJ_TOP}
}

pause() {
    while true; do
        read -p "Continue? [Y/N] " yn
        case $yn in
            [Yy]* ) break;;
            [Nn]* ) exit;;
            * ) echo "Please press [Y]-yes or [N]-no.";;
        esac
    done
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
pause
sh ${PRJ_TOP}/tool/clean.sh

source setup.env && rebuild "-DCMAKE_BUILD_TYPE=Debug -DTEST_ONLY=1 -DUSE_REGISTER=1 -DINCLUDE_TB_CMN=1" > ${PRJ_TOP}/build.log 2>&1
if [ ! -e "${PRJ_TOP}/build/model1.elf" ]; then
    echo ".elf file was NOT found!"
    exit 1
fi
info
pause
sh ${PRJ_TOP}/tool/clean.sh

source setup.env STM32F405RGT6 emulator && rebuild "-DCMAKE_BUILD_TYPE=Debug -DTEST_ONLY=1 -DUSE_REGISTER=1 -DINCLUDE_TB_CMN=1" > ${PRJ_TOP}/build.log 2>&1
if [ ! -e "${PRJ_TOP}/build/model1.elf" ]; then
    echo ".elf file was NOT found!"
    exit 1
fi
info
pause
sh ${PRJ_TOP}/tool/clean.sh


# Run `qemu-system-arm -M help` for help
# qemu-system-arm -M netduinoplus2 -m 1024 -smp 1 -cpu cortex-m4 -nographic -semihosting-config enable=on,target=native -kernel ${PRJ_TOP}/build/model1.elf

# qemu-system-arm -M stm32-f411ce -semihosting -kernel ${PRJ_TOP}/build/model1.bin
# qemu-system-arm -cpu cortex-m3 -machine lm3s6965evb -nographic -semihosting-config enable=on,target=native -kernel your_program.elf

echo "Success. Return 0"
exit 0