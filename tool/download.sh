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


generate_jlink_script() {
    if [ -e "${PRJ_TOP}/tool/generated/jlink.flash" ]; then
        rm "${PRJ_TOP}/tool/generated/jlink.flash"
    fi

    echo "connect"                                            > "${PRJ_TOP}/tool/generated/jlink.flash"
    echo "loadbin   ${PRJ_TOP}/build/model1.bin, 0x08000000" >> "${PRJ_TOP}/tool/generated/jlink.flash"
    echo "verifybin ${PRJ_TOP}/build/model1.bin, 0x08000000" >> "${PRJ_TOP}/tool/generated/jlink.flash"
    echo "reset"                                             >> "${PRJ_TOP}/tool/generated/jlink.flash"
    echo "go"                                                >> "${PRJ_TOP}/tool/generated/jlink.flash"
    echo "exit"                                              >> "${PRJ_TOP}/tool/generated/jlink.flash"

}

download_via_jlink() {
    generate_jlink_script
    /usr/local/bin/JLinkExe -if swd -device STM32F411CE -AutoConnect 1 -speed 4000 -CommanderScript "${PRJ_TOP}/tool/generated/jlink.flash" > ${PRJ_TOP}/tool/outputs/jlink.log 2>&1
}


download_via_stlink() {
    # command line:   ./st-flash [--debug] [--reset] [--connect-under-reset] [--hot-plug] [--opt] [--serial <serial>] [--format <format>] [--flash=<fsize>] [--freq=<kHz>] [--area=<area>] {read|write} [path] [addr] [size]
    # command line:   ./st-flash [--debug] [--connect-under-reset] [--hot-plug] [--freq=<kHz>] [--serial <serial>] erase [addr] [size]
    # command line:   ./st-flash [--debug] [--freq=<kHz>] [--serial <serial>] reset
    echo $(st-flash --version)
    st-flash --debug --reset write ${PRJ_TOP}/build/model1.bin 0x08000000 > ${PRJ_TOP}/tool/outputs/stlink.log 2>&1
}


################################################################
# Main
################################################################
echo "--- Start downloading to the target ---"

rm -rf ${PRJ_TOP}/tool/generated && mkdir ${PRJ_TOP}/tool/generated
rm -rf ${PRJ_TOP}/tool/outputs   && mkdir ${PRJ_TOP}/tool/outputs

if [ $1 == "--adaptor" ]; then
    if [ $2 == "jlink" ]; then
        echo "Download via J-Link..."
        download_via_jlink
    elif [ $2 == "stlink" ]; then
        echo "Download via ST-Link..."
        download_via_stlink
    else
        echo "Unknown adaptor $1"
        exit 1
    fi
else
    echo "Please provide the adaptor: --adaptor < stlink | jlink >"
    echo "Given args are $1 $2"
fi


echo "--- End of the target download ---"