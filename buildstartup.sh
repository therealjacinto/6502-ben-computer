#!/bin/bash
SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

printf "INFO: Building source...\n"
${SCRIPTPATH}/bin/vasm6502_oldstyle ${SCRIPTPATH}/targets/libsrc/6502-be/startup.s -o ${SCRIPTPATH}/lib/startup.o -Fvobj -quiet

printf "\nINFO: Script Complete!\n"