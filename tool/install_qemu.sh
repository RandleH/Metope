#!/bin/bash
################################################################
# Get current script directory
################################################################
if [ -z "${PRJ_TOP+x}" ]; then
    DIR_CWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    PRJ_TOP=${DIR_CWD}/..
else
    echo "PRJ_TOP was set to ${PRJ_TOP}."
fi


install_sourcecode_from_HTTP() {
    cd ${PRJ_TOP}/lib
    if [ ! -e "${PRJ_TOP}/lib/qemu-10.1.0.tar.xz" ]; then
        wget https://download.qemu.org/qemu-10.1.0.tar.xz
        tar xvJf qemu-10.1.0.tar.xz -C qemu
    fi
    cd qemu-10.1.0
    ./configure
    make
    cd ${PRJ_TOP}
}

install_sourcecode_from_GitLab() {
    cd ${PRJ_TOP}/lib
    if [ ! -e "${PRJ_TOP}/lib/qemu" ]; then
        git clone https://gitlab.com/qemu-project/qemu.git
    fi

    cd qemu
    ./configure
    make
    cd ${PRJ_TOP}
}

install() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # https://www.qemu.org/download/#macos
        echo "[qemu]: Detect os environment is $OSTYPE. Installing the binary..."
        pause
        brew install qemu
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # https://www.qemu.org/download/#linux
        echo "[qemu]: Detect os environment is $OSTYPE which is NOT supported."
        pause
        sudo apt-get install qemu-system
    elif [[ "$OSTYPE" == "win32" ]]; then
        # https://www.qemu.org/download/#windows
        echo "[qemu]: Detect os environment is $OSTYPE which is NOT supported."
    else
        # https://www.qemu.org/download/#source
        echo "[qemu]: Detect unknown os environment. OS type is $OSTYPE. Preparing for the native build..."
        pause
        install_sourcecode_from_HTTP
        # install_sourcecode_from_GitLab
    fi
    cd ${PRJ_TOP}
}


################################################################
# Main Program
################################################################
cd ${PRJ_TOP}
install