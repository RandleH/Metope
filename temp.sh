#!/bin/bash
################################################################
# Get current script directory
################################################################

if [ $1 == "--adaptor" ]; then
    echo $1
fi
echo $2



foo() {
    JLINK_PORT=9
}


foo
echo $JLINK_PORT
