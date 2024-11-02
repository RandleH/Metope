DIR_CWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd ${DIR_CWD}

cd ./build

make -j32
arm-none-eabi-size ${DIR_CWD}/build/model1.elf