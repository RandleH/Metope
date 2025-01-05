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

# LOG_LEVEL: 0=Warning/Error; 1={Inherit}/Info; 2={Inherit}/Debug;

cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_REGISTER=1 -DLOG_LEVEL=2 ..
# cmake -DCMAKE_BUILD_TYPE=Release -DUSE_REGISTER=1 ..

# cmake -DCMAKE_BUILD_TYPE=Debug ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DTEST_ONLY=1 -DUSE_REGISTER=1 ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_REGISTER=1 ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DTEST_ONLY=1 -DUSE_REGISTER=1 -DINCLUDE_TB_OS=1  ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DTEST_ONLY=1 -DUSE_REGISTER=1 -DINCLUDE_TB_CMN=1 -DQMI8658A=1 ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DTEST_ONLY=1 -DUSE_REGISTER=1 -DINCLUDE_TB_BSP=1 -DQMI8658A=1 ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DTEST_ONLY=1 -DUSE_REGISTER=1 -DINCLUDE_TB_OS=1  ..


################################################################
# Execution Zone
################################################################
make -j32

################################################################
# Backup Auto-Generated Files
################################################################
