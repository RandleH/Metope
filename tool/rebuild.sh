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


rm -rf ${PRJ_TOP}/build && mkdir ${PRJ_TOP}/build

cd ${PRJ_TOP}/build

################################################################
# Compilation Configuration Zone
################################################################
# LOG_LEVEL: 0=Warning/Error; 1={Inherit}/Info; 2={Inherit}/Debug;

# cmake -DCMAKE_BUILD_TYPE=Debug -DLOG_LEVEL=2 ${PRJ_TOP}
# cmake -DCMAKE_BUILD_TYPE=Release ..

# cmake -DCMAKE_BUILD_TYPE=Debug ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DUNIT_TEST=1 ..

# cmake -DCMAKE_BUILD_TYPE=Debug ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DUNIT_TEST=1 -DINCLUDE_TB_OS=1  ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DUNIT_TEST=1 -DINCLUDE_TB_CMN=1 -DQMI8658A=1 ..
cmake -DCMAKE_BUILD_TYPE=Debug -DUNIT_TEST=1 -DINCLUDE_TB_CMN=1 -DQMI8658A=1 ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DUNIT_TEST=1 -DINCLUDE_TB_BSP=1 -DQMI8658A=1 ..

# cmake -DCMAKE_BUILD_TYPE=Debug -DUNIT_TEST=1 -DINCLUDE_TB_OS=1 ..

################################################################
# Execution Zone
################################################################
make -j32


cd ${PRJ_TOP}