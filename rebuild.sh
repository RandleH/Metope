#!/bin/bash
################################################################
# Get current script directory
################################################################
DIR_CWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd ${DIR_CWD}
. clean.sh

################################################################
# Create project compilation folder
################################################################
mkdir build
cd ./build

################################################################
# Compilation Configuration Zone
################################################################
cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_REGISTER=1 ..
# cmake -DCMAKE_BUILD_TYPE=Release -DCHIP=STM32F411CEU6 -DUSE_REGISTER=1 ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DCHIP=STM32F411CEU6 ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DCHIP=STM32F411CEU6 -DTEST_ONLY=1 -DUSE_REGISTER=1 ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DCHIP=STM32F411CEU6 -DUSE_REGISTER=1 ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DCHIP=STM32F411CEU6 -DTEST_ONLY=1 -DUSE_REGISTER=1 -DINCLUDE_TB_OS=1  ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DCHIP=STM32F411CEU6 -DTEST_ONLY=1 -DUSE_REGISTER=1 -DINCLUDE_TB_CMN=1 -DQMI8658A=1 ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DCHIP=STM32F411CEU6 -DTEST_ONLY=1 -DUSE_REGISTER=1 -DINCLUDE_TB_BSP=1 -DQMI8658A=1 ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DCHIP=STM32F405RGT6 -DTEST_ONLY=1 -DUSE_REGISTER=1 -DINCLUDE_TB_OS=1  ..


################################################################
# Execution Zone
################################################################
make -j32

################################################################
# Backup Auto-Generated Files
################################################################
