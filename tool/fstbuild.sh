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


################################################################
# Execution Zone
################################################################
cd ${PRJ_TOP}/build

if [ $# -gt 0 ]; then
    make -j$1
else
    make -j32
fi