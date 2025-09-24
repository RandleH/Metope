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


_generate_gdb_memory_dump_script() {
    echo "dump memory ${PRJ_TOP}/dbg/outputs/main_mem_sector0.bin 0x08000000 0x08003FFF" >> $1
    echo "dump memory ${PRJ_TOP}/dbg/outputs/main_mem_sector1.bin 0x08004000 0x08007FFF" >> $1
    echo "dump memory ${PRJ_TOP}/dbg/outputs/main_mem_sector2.bin 0x08008000 0x0800BFFF" >> $1
    echo "dump memory ${PRJ_TOP}/dbg/outputs/main_mem_sector3.bin 0x0800C000 0x0800FFFF" >> $1
    echo "dump memory ${PRJ_TOP}/dbg/outputs/main_mem_sector4.bin 0x08010000 0x0801FFFF" >> $1
    echo "dump memory ${PRJ_TOP}/dbg/outputs/main_mem_sector5.bin 0x08020000 0x0803FFFF" >> $1
    echo "dump memory ${PRJ_TOP}/dbg/outputs/main_mem_sector6.bin 0x08040000 0x0805FFFF" >> $1
    echo "dump memory ${PRJ_TOP}/dbg/outputs/main_mem_sector7.bin 0x08060000 0x0807FFFF" >> $1
    echo "dump memory ${PRJ_TOP}/dbg/outputs/system_mem.bin       0x1FFF0000 0x1FFF77FF" >> $1
    echo "dump memory ${PRJ_TOP}/dbg/outputs/sram_mem.bin         0x20000000 0x2001FFFF" >> $1
    echo "dump memory ${PRJ_TOP}/dbg/outputs/periph_mem.bin       0x40000000 0x5003FFFF" >> $1
}

generate_gdb_hardfault_detection_script() {
    if [ -e "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb" ]; then
        rm "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
    fi
    echo "pwd"                                          >> "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
    echo "set pagination off"                           >> "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
    echo "target extended-remote localhost:${GDB_PORT}" >> "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
    echo "monitor reset"                                >> "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
    echo "b main"                                       >> "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
    echo "continue"                                     >> "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
    echo "b HardFault_Handler"                          >> "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
    echo "define hook-stop"                             >> "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
    echo "  backtrace full"                             >> "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
    _generate_gdb_memory_dump_script                       "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
    echo "  quit"                                       >> "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
    echo "end"                                          >> "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
    echo "continue"                                     >> "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
    echo "quit"                                         >> "${PRJ_TOP}/dbg/generated/catch_hardfault.gdb"
}

run_stlink() {
    echo $(st-flash --version)
    echo $(st-util --version)
    st-util -p 4242 -v  > ${PRJ_TOP}/dbg/outputs/stlink.log 2>&1
}

run_jlink() {
    /usr/local/bin/JLinkGDBServer -singlerun -nogui -if swd -port 50000 -swoport 50001 -telnetport 50002 -device STM32F411CE > ${PRJ_TOP}/dbg/outputs/jlink.log 2>&1
}

run_gdb() {
    ${METOPE_ARM_TOOLCHAIN_REPO}/bin/arm-none-eabi-gdb -batch -x ${PRJ_TOP}/dbg/generated/catch_hardfault.gdb ${PRJ_TOP}/build/model1.elf > ${PRJ_TOP}/dbg/outputs/gdb.log 2>&1
}

run_uart() {
    UART_NAME="screen-usbserial"
    UART_DEV=$(ls -a /dev | grep 'tty.usbserial-' -m 1)
    screen -d -m -S $UART_NAME -L /dev/$UART_DEV 115200
}

close_uart() {
    screen -r $1 -X quit
    mv ${PRJ_TOP}/screenlog.0 ${PRJ_TOP}/dbg/outputs/uart_outputs.log
}


################################################################
# Main
################################################################
echo "--- Start monitoring the target ---"

rm -rf ${PRJ_TOP}/dbg/generated && mkdir ${PRJ_TOP}/dbg/generated
rm -rf ${PRJ_TOP}/dbg/outputs   && mkdir ${PRJ_TOP}/dbg/outputs


if [ $1 == "--adaptor" ]; then
    if [ $2 == "jlink" ]; then
        GDB_PORT=50000
        run_jlink &
        PID_ADAPTOR=$!
        echo "Launch J-Link server PID=${PID_ADAPTOR}. GDB Port was set to ${GDB_PORT}"
    elif [ $2 == "stlink" ]; then
        GDB_PORT=4242
        run_stlink &
        PID_ADAPTOR=$!
        echo "Launch ST-Link server PID=${PID_ADAPTOR}. GDB Port was set to ${GDB_PORT}"
    else
        echo "Unknown adaptor $1"
        exit 1
    fi
else
    echo "Please provide the adaptor: --adaptor < stlink | jlink >"
fi

generate_gdb_hardfault_detection_script

run_uart &
PID_UART=$!

run_gdb &
PID_GDB=$!

echo "Launch UART listener PID=${PID_UART}"
echo "Launch GDB server PID=${PID_GDB}"

wait $PID_GDB
wait $PID_ADAPTOR
close_uart $UART_NAME

echo "--- End of the monitor program ---"
