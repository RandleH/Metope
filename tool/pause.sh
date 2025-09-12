#!/bin/bash

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

pause