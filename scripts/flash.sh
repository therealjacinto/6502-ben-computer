#!/bin/bash
SCRIPTSPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
REPOPATH="${SCRIPTSPATH}/.."

if [[ "$#" -ne 1 ]]; then
    printf "ERROR: this script requires the path of the binary to flash\n"
    exit 0
fi

minipro -p AT28C256 -w ${1}
