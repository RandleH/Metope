#!/bin/bash
################################################################
# Get current script directory
################################################################
if [[ -n "$PRJ_TOP" ]]; then
    echo "PRJ_TOP was set to ${PRJ_TOP}."
else
    DIR_CWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    PRJ_TOP=${DIR_CWD}/..
fi

cd ${PRJ_TOP}

echo "Clean working directory: ${PRJ_TOP}"

rm -rf ${PRJ_TOP}/build
rm     ${PRJ_TOP}/main.cc
rm     ${PRJ_TOP}/CMakeLists.txt
rm     ${PRJ_TOP}/*.log
rm -rf ${PRJ_TOP}/STM32CubeMX/

cd ${PRJ_TOP}