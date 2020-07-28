#!/bin/bash

# Get path of script
SCRIPTSPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
REPOPATH="${SCRIPTSPATH}/.."
mkdir ${REPOPATH}/build

export VBCC=${REPOPATH}
${SCRIPTSPATH}/buildstartup.sh
${REPOPATH}/bin/vc +be ${REPOPATH}/src/hello.c ${REPOPATH}/lib/lcdio/lcdio.c -o ${REPOPATH}/build/hello