#!/bin/bash
SCRIPTSPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
REPOPATH="${SCRIPTSPATH}/.."

printf "INFO: Building source...\n"
${REPOPATH}/bin/vasm6502_oldstyle ${REPOPATH}/targets/libsrc/6502-be/startup.s -o ${REPOPATH}/lib/startup.o -Fvobj -quiet

printf "\nINFO: Script Complete!\n"