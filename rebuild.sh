
DIR_CWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd ${DIR_CWD}
. clean.sh


mkdir build
cd ./build

cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j32




