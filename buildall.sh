#!/bin/bash

# Get path of script
SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
mkdir ${SCRIPTPATH}/build

export VBCC=${SCRIPTPATH}
${SCRIPTPATH}/buildstartup.sh
${SCRIPTPATH}/bin/vc +be ${SCRIPTPATH}/src/hello.c ${SCRIPTPATH}/lib/lcdio/lcdio.c -o ${SCRIPTPATH}/build/hello