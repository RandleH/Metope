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
sh "${PRJ_TOP}/tool/pause.sh"


rm -rf build
rm     main.cc
rm     CMakeLists.txt
rm     *.log
rm -rf STM32CubeMX/

cd ${PRJ_TOP}