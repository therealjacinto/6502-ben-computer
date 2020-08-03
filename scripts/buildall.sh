#!/bin/bash

# Get path of script
SCRIPTSPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
REPOPATH="${SCRIPTSPATH}/.."
mkdir -p ${REPOPATH}/build

export VBCC=${REPOPATH}
${SCRIPTSPATH}/buildmanually.sh --startup
${REPOPATH}/bin/vc +be ${REPOPATH}/src/hello.c ${REPOPATH}/lib/lcdio/lcdio.c -o ${REPOPATH}/build/hello
