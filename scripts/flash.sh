#!/bin/bash
SCRIPTSPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
REPOPATH="${SCRIPTSPATH}/.."

minipro -p AT28C256 -w ${REPOPATH}/build/hello