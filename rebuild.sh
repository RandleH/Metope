
DIR_CWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd ${DIR_CWD}
. clean.sh


mkdir build
cd ./build

# cmake -DCMAKE_BUILD_TYPE=Debug -DCHIP=STM32F411CEU6  ..
cmake -DCMAKE_BUILD_TYPE=Debug -DCHIP=STM32F411CEU6 -DTEST_ONLY=1 ..
make -j32

# Backup Auto-Generated Files
# cp -rf ${DIR_CWD}/STM32CubeMX/STM32F405RGT6 ${DIR_CWD}/backup/STM32F405RGT6
cp -rf ${DIR_CWD}/STM32CubeMX/STM32F411CEU6 ${DIR_CWD}/backup/STM32F411CEU6