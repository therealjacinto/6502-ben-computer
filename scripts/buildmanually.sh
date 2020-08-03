#!/bin/bash
SCRIPTSPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
REPOPATH="${SCRIPTSPATH}/.."
assembly_startup_code="${REPOPATH}/targets/libsrc/6502-be/startup.s"

if [[ "$#" -lt 1 ]]; then
    printf "ERROR: this script requires flags to determine what to build:\n"
    printf "\t--startup\t\tGenerate assembler output for startup code\n"
    printf "\t--assembler\t\tGenerate assember output for src/ code\n"
    printf "\t--objects\t\tCompile object files for assembler code\n"
    printf "\t--linking\t\tLink object files into raw binary\n"
    printf "\t--save-output-files\tDo not delete intermediate files\n"
    printf "\t--all\t\t\tSame as --startup, --assembler, --objects, and"
    printf " --linking. This is essentially running the buildall script.\n"
    exit 0
fi

while [ $# -gt 0 ]; do
    case $1 in
        "--startup")
            startupc=true
            ;;
        "--assembler")
            assembler=true
            ;;
        "--objects")
            objects=true
            ;;
        "--linking")
            linking=true
            ;;
        "--save-output-files")
            saveoutput=true
            ;;
        "--all")
            startupc=true
            assembler=true
            objects=true
            ;;
    esac
    shift
done


if [[ ${startupc} ]]; then
    printf "INFO: Generating startup code assembler output...\n"
    ${REPOPATH}/bin/vbcc6502 ${REPOPATH}/targets/libsrc/6502-be/startup.c -o=${REPOPATH}/build/startup.s -c99 -quiet -avoid-bank-switch -O=23999
    ${REPOPATH}/bin/vbcc6502 ${REPOPATH}/targets/libsrc/6502-be/startup.h -o=${REPOPATH}/build/startup-reg.s -c99 -quiet -avoid-bank-switch -O=16384 
    printf "INFO: Compiling startup object files...\n"
    ${REPOPATH}/bin/vasm6502_oldstyle ${REPOPATH}/build/startup.s -o ${REPOPATH}/lib/startup.o -Fvobj -quiet -nowarn=62
    ${REPOPATH}/bin/vasm6502_oldstyle ${REPOPATH}/build/startup-reg.s -o ${REPOPATH}/lib/startup-reg.o -Fvobj -quiet -nowarn=62
fi

if [[ ${assembler} ]]; then
    printf "INFO: Generating assembler output...\n"
    ${REPOPATH}/bin/vbcc6502 -I${REPOPATH}/lib ${REPOPATH}/lib/lcdio/lcdio.c -o=${REPOPATH}/build/lcdio.s -c99 -quiet -avoid-bank-switch -O=1
    ${REPOPATH}/bin/vbcc6502 -I${REPOPATH}/lib ${REPOPATH}/src/hello.c -o=${REPOPATH}/build/hello.s -c99 -quiet -avoid-bank-switch -O=1
fi


if [[ ${objects} ]]; then
    printf "INFO: Compiling object files...\n"
    ${REPOPATH}/bin/vasm6502_oldstyle ${REPOPATH}/build/lcdio.s -o ${REPOPATH}/build/lcdio.o -Fvobj -quiet -nowarn=62
    ${REPOPATH}/bin/vasm6502_oldstyle ${REPOPATH}/build/hello.s -o ${REPOPATH}/build/hello.o -Fvobj -quiet -nowarn=62
fi

if [[ ${linking} ]]; then
    printf "INFO: Linking object files into raw binary...\n"
    ${REPOPATH}/bin/vlink -b rawbin1 -Cvbcc -T${REPOPATH}/targets/6502-be/vlink.cmd ${REPOPATH}/lib/startup.o -set-deluscore ${REPOPATH}/lib/startup-reg.o -clr-deluscore ${REPOPATH}/build/hello.o ${REPOPATH}/build/lcdio.o -o ${REPOPATH}/build/hello
fi

if [[ ! ${saveoutput} ]]; then
    printf "INFO: Deleting intermediate files...\n"
    rm -f ${REPOPATH}/build/*.o ${REPOPATH}/build/*.s
fi
printf "INFO: Build Complete\n"