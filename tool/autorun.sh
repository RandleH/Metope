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


pause() {
    while true; do
        read -p "Continue? [Y/N] " yn
        case $yn in
            [Yy]* ) break;;
            [Nn]* ) exit;;
            * ) echo "Please press [Y]-yes or [N]-no.";;
        esac
    done
}

download() {
    st-info --probe
    st-flash --version
    st-flash --reset write $1 0x08000000
}


while true; do
    if [ $# -gt 0 ]; then
        echo "Sleep for $1 seconds..."
        sleep $1
    else
        echo "Sleep for 10 minutes..."
        sleep 600
    fi
    echo "Check remote master branch status..."
    git fetch
    REMOTE_UPDATES=$(git rev-list --count HEAD..origin/master)
    if [ "$REMOTE_UPDATES" -gt 0 ]; then
        echo "Remote branch 'origin/master' has updates."
        git pull
        sh tool/clean.sh
        source setup.env && sh tool/rebuild.sh
        if [ ! -e "${PRJ_TOP}/build/model1.bin" ]; then
            echo ".bin file was NOT found!"
            pause
        else
            download ${PRJ_TOP}/build/model1.bin
        fi
    else
        echo "Local branch is up to date with 'origin/master'."
    fi
done



