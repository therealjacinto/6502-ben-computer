#!/bin/bash
SCRIPTSPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
REPOPATH="${SCRIPTSPATH}/.."

printf "INFO: Compiling startup C code...\n"
${REPOPATH}/bin/vbcc6502 ${REPOPATH}/targets/libsrc/6502-be/startup.c -o=${REPOPATH}/build/startup.s -c99
${REPOPATH}/bin/vbcc6502 ${REPOPATH}/targets/libsrc/6502-be/startup.h -o=${REPOPATH}/build/startup-res.s -c99 -O=16384
${REPOPATH}/bin/vbcc6502 -quiet -avoid-bank-switch -I${REPOPATH}/lib ${REPOPATH}/lib/lcdio/lcdio.c -o=${REPOPATH}/build/lcdio.s -O=1
${REPOPATH}/bin/vbcc6502 -quiet -avoid-bank-switch -Ilib ${REPOPATH}/src/hello.c -o=${REPOPATH}/build/hello.s -O=1

printf "INFO: Building source...\n"
${REPOPATH}/bin/vasm6502_oldstyle ${REPOPATH}/build/startup.s -o ${REPOPATH}/build/startup.o -Fvobj -quiet -nowarn=62
#${REPOPATH}/bin/vasm6502_oldstyle ${REPOPATH}/targets/libsrc/6502-be/startup.s -o ${REPOPATH}/build/startup.o -Fvobj -quiet -nowarn=62
${REPOPATH}/bin/vasm6502_oldstyle ${REPOPATH}/build/startup-res.s -o ${REPOPATH}/build/startup-res.o -Fvobj -quiet -nowarn=62
${REPOPATH}/bin/vasm6502_oldstyle ${REPOPATH}/build/lcdio.s -o ${REPOPATH}/build/lcdio.o -Fvobj -quiet -nowarn=62
${REPOPATH}/bin/vasm6502_oldstyle ${REPOPATH}/build/hello.s -o ${REPOPATH}/build/hello.o -Fvobj -quiet -nowarn=62

printf "INFO: Linking...\n"
${REPOPATH}/bin/vlink -b rawbin1 -Cvbcc -T${REPOPATH}/targets/6502-be/vlink.cmd ${REPOPATH}/build/startup.o -set-deluscore ${REPOPATH}/build/startup-res.o -clr-deluscore ${REPOPATH}/build/hello.o ${REPOPATH}/build/lcdio.o -o ${REPOPATH}/build/hello

#printf "INFO: Building source...\n"
#${REPOPATH}/bin/vasm6502_oldstyle ${REPOPATH}/targets/libsrc/6502-be/startup-test.s -o ${REPOPATH}/lib/startup.o -Fvobj -quiet -nowarn=62

#printf "INFO: Script Complete!\n"

#printf "INFO: Compiling reserve code\n"
#${REPOPATH}/bin/vbcc6502 ${REPOPATH}/targets/libsrc/6502-be/startup.h -o=${REPOPATH}/targets/libsrc/6502-be/startup-res.s -c99 -O=16384
#printf "INFO: Building reserve source...\n"
#${REPOPATH}/bin/vasm6502_oldstyle ${REPOPATH}/targets/libsrc/6502-be/startup-res.s -o ${REPOPATH}/lib/startup-res.o -Fvobj -quiet -nowarn=62
