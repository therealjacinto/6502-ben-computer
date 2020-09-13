#!/bin/bash

SCRIPTSPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
REPOPATH="${SCRIPTSPATH}/.."


if [[ "$#" -lt 1 ]]; then
    printf "ERROR: This script requires the C file as an argument"
    exit
fi
if [ ! -f ${1} ]; then
    printf "ERROR: the file you have inputted does not exist.\n"
    exit 1
fi

c_file=${1}
c_file_base=$(basename ${c_file} | sed 's/\.c//g')

starting_string="101110110111111"

function bin_to_dec ()
{
    echo "$((2#${1}))"
}

function unset_bit ()
{
    string=${1}
    index=${2}
    index_next=$((${2}+1))
    if [ "${string:${index}:1}" = "0" ]; then
        echo ${string}
    else
        echo "${string:0:${index}}0${string:${index_next}}"
    fi
}

${REPOPATH}/bin/vbcc6502 -I${REPOPATH}/lib ${c_file} -o=${REPOPATH}/build/${c_file_base}.s -c99 -quiet -avoid-bank-switch -O=$(bin_to_dec ${starting_string})
base=$(wc -l ${REPOPATH}/build/${c_file_base}.s | awk -F " " '{ print $1 }')
last=$((${base}-1))
winning_string=${starting_string}

while [ ${last} -lt ${base} ] || [ "$(bin_to_dec ${winning_string})" -lt "$(bin_to_dec ${starting_string})" ]; do
    last=${base}
    starting_string=${winning_string}
    base=$(wc -l ${REPOPATH}/build/${c_file_base}.s | awk -F " " '{ print $1 }')
    for (( i=0; i<${#starting_string}; i++ )); do
        test_string=$(unset_bit ${starting_string} ${i})
        suppress=$(${REPOPATH}/bin/vbcc6502 -I${REPOPATH}/lib ${c_file} -o=${REPOPATH}/build/${c_file_base}.s -c99 -quiet -avoid-bank-switch -O=$(bin_to_dec ${test_string}))
        current=$(wc -l ${REPOPATH}/build/${c_file_base}.s | awk -F " " '{ print $1 }')
        if [ "${current}" -ne "0" ] && [ "${current}" -le "${last}" ] && [ "$(bin_to_dec ${test_string})" -lt "$(bin_to_dec ${winning_string})" ]; then
            last=${current}
            winning_string=${test_string}
            break
        fi
    done
done

printf "INFO: Script complete! The smallest optimization found was:\n\t"
printf "$(bin_to_dec ${winning_string})\n"