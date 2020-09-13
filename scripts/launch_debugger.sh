#!/bin/bash
set -e

SCRIPTSPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
REPOPATH="${SCRIPTSPATH}/.."
CONDA_PATH="${HOME}/miniconda3"
ENTRY_POINT=7ffc
JMP_CODE=4c
SCRIPT=${0}

if [[ "$#" -lt 1 ]]; then
    printf "ERROR: this script requires the path of the binary to debug\n"
    exit 1
elif [ ! -f ${1} ]; then
    printf "ERROR: the file you have inputted does not exist.\n"
    exit 1
fi
FILE_PATH=${1}
DISASSEMBLE_FILE="${REPOPATH}/build/disassemble.out"
rm -f ${DISASSEMBLE_FILE}

while [ $# -gt 0 ]; do
    case $1 in
        "--verbose")
            verbose=true
            ;;
        "--find-labels")
            find_labels=true
            ;;
        "--disassemble-only")
            disassemble_only=true
            ;;
        "--conda-path")
            shift
            manual_conda_path=$1
            ;;
        "--asm-directory")
            shift
            asm_directory=$1
            ;;
    esac
    shift
done

# Check if expect is installed
if ! command -v expect &> /dev/null; then
    printf "ERROR: expect is not installed. Please install it before running "
    printf "this script.\n"
    exit 1
fi

# Check if conda is installed
if [[ ! ${manual_conda_path} ]]; then
    if ! command -v conda &> /dev/null; then
        printf "ERROR: conda is not installed. Please install it before running "
        printf "this script. And install the conda environment by running the "
        printf "install_conda_env.sh script.\n"
        exit 1
    fi
fi

if [[ ${verbose} ]]; then
    printf "INFO: Starting Debugger...\n"
fi

function convert_2B_BE_TO_LE ()
{
    echo ${1:2:2}${1:0:2}
}

function get_2B_from_file_at_addr ()
{
    echo $(xxd -s ${1} -l 2 -p ${FILE_PATH})
}

function convert_hex_to_dec ()
{
    echo $((16#${1}))
}

function convert_dec_to_hex ()
{
    printf "%x\n" ${1} 
}

if [[ ! ${manual_conda_path} ]]; then
    # Find path of the emulator
    if [[ ${verbose} ]]; then
        printf "INFO: Activating conda environment...\n"
    fi
    source ${CONDA_PATH}/etc/profile.d/conda.sh
    # Check if conda environment has been installed
    if ! conda env list | grep 6502 &> /dev/null; then
        printf "ERROR: 6502 conda environment has not been installed. Please run "
        printf "the install_conda_env.sh script before running this script.\n"
        exit 1
    fi
    conda activate 6502
    PY65MON_PATH=$(dirname $(which python))/py65mon
else
    PY65MON_PATH=${manual_conda_path}/py65mon
fi
if [[ ${verbose} ]]; then
    printf "INFO: py65mon path found! (${PY65MON_PATH})\n"
fi

# Disassemble ROM and save to DISASSEMBLE_FILE
if [[ ${find_labels} ]] || [[ ${disassemble_only} ]]; then
    if [[ ${verbose} ]]; then
        printf "INFO: Attempting to find labels\n"
    fi
    label_locations="build/locations.csv"
    rm -f ${DISASSEMBLE_FILE}
    rm -f ${label_locations}
    expect -c "
        log_user 0
        set timeout 5
        set prompt {\.$}
        spawn ${PY65MON_PATH}

        expect -re \$prompt
        send \"load ${FILE_PATH} 8000\n\"
        expect -re \$prompt

        foreach register {0 1 2 3 4 5 6 7 8 9 a b c d e f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f} number {0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31} {
            send \"add_label \$register r\$number\n\"
            expect -re \$prompt
        }

        send \"add_label 20 sp\n\"
        expect -re \$prompt

        foreach register {22 26 2a 2e} number {0 1 2 3} {
            send \"add_label \$register btmp\$number\n\"
            expect -re \$prompt
        }

        log_file -a ${DISASSEMBLE_FILE}
        send \"disassemble 8000:ffff\n\"
        expect -re \$prompt
        log_file
    "
    sed -i '1d' ${DISASSEMBLE_FILE}
    sed -i '$ d' ${DISASSEMBLE_FILE}
    sed -i '$ d' ${DISASSEMBLE_FILE}
    sed -i '$ d' ${DISASSEMBLE_FILE}
    sed -i '$ d' ${DISASSEMBLE_FILE}

    if [[ ${disassemble_only} ]]; then
        exit
    fi

    # Parse ROM with assembly files in build directory
    if [[ ${verbose} ]]; then
        printf "INFO: calling python parsefile script with the following "
        printf "arguments: ${FILE_PATH} ${DISASSEMBLE_FILE} "
        printf "${label_locations}\n"
    fi
    if [[ ${manual_conda_path} ]]; then
        python_path=${manual_conda_path}/python
    else
        python_path=python
    fi
    if [[ ${asm_directory} ]]; then
        search_directory=${asm_directory}
    else
        search_directory=$(dirname ${FILE_PATH})
    fi
    ${python_path} scripts/python/parseFile.py ${FILE_PATH} ${search_directory} ${DISASSEMBLE_FILE} ${label_locations}
    if [ -f ${label_locations} ]; then
        if [[ ${verbose} ]]; then
            printf "INFO: label locations parsed and outputted to "
            printf "INFO: ${label_locations}. Incorporating them into "
            printf "debugger...\n"
        fi
        method_locations=()
        method_names=()
        while IFS= read -r line; do
            location=$(echo ${line} | awk -F "," '{ print $2 }')
            name=$(echo ${line} | awk -F "," '{ print $1 }')
            method_locations+=(${location})
            method_names+=(${name})
        done < "${label_locations}"
    fi
fi

# Looking for infinite jmp loops
if [[ ${verbose} ]]; then
    printf "INFO: Looking for infinite jmp loops\n"
fi
locations=( $(grep -obaP "\x${JMP_CODE}" ${FILE_PATH}) )
infinite_loops=()
for i in "${locations[@]}"; do
	opcode_address=$(echo ${i} | awk -F ":" '{ print $1 }')
    address_address=$(( opcode_address + 1 ))
    jmp_address_BE=$(get_2B_from_file_at_addr ${address_address})
    jmp_address_machine_hex=$(convert_2B_BE_TO_LE ${jmp_address_BE})
    jmp_address_machine=$(convert_hex_to_dec ${jmp_address_machine_hex})
    jmp_address=$(( ${jmp_address_machine} - $(convert_hex_to_dec 8000) ))
    if [[ "${opcode_address}" == "${jmp_address}" ]]; then
        infinite_loops+=(${jmp_address_machine_hex})
    fi
done
if [ ! -z "${infinite_loops}" ] && [[ ${verbose} ]]; then
    printf "INFO: Infinite jmp loop detected. A breakpoint will be added at the following addresses:\n"
    printf "\t0x%s\n" "${infinite_loops[@]}" 
fi

# Get starting address
ENTRY_POINT_DEC=$(convert_hex_to_dec ${ENTRY_POINT})
START_ADDR_BE=$(get_2B_from_file_at_addr ${ENTRY_POINT_DEC})
START_ADDR_LE=$(convert_2B_BE_TO_LE ${START_ADDR_BE})

if [[ ${verbose} ]]; then
    printf "INFO: Initiating py65mon. Loading file (${1}) at machine address "
    printf "0x${START_ADDR_LE}. You may now start using the emulator. Visit "
    printf "https://py65.readthedocs.io for docs on how to use it, and enter 'q' "
    printf "to exit.\n"
fi

LOG_USER=0
if [[ ${verbose} ]]; then
    LOG_USER=1
fi

expect -c "
    log_user ${LOG_USER}
    set timeout 5
    set prompt {\.$}
    spawn ${PY65MON_PATH}

    expect -re \$prompt
    send \"load ${FILE_PATH} 8000\n\"
    expect -re \$prompt
    send \"fill 0000 ${JMP_CODE}\n\"
    expect -re \$prompt
    send \"fill 0001 ${START_ADDR_LE:2:2}\n\"
    expect -re \$prompt
    send \"fill 0002 ${START_ADDR_LE:0:2}\n\"
    expect -re \$prompt
    send \"step\n\"
    expect -re \$prompt

    foreach register {0 1 2 3 4 5 6 7 8 9 a b c d e f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f} number {0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31} {
        send \"add_label \$register r\$number\n\"
        expect -re \$prompt
    }

    send \"add_label 20 sp\n\"
    expect -re \$prompt

    foreach register {22 26 2a 2e} number {0 1 2 3} {
        send \"add_label \$register btmp\$number\n\"
        expect -re \$prompt
    }

    send \"add_label 100 stack\n\"
    expect -re \$prompt
    send \"add_label 200 ram\n\"
    expect -re \$prompt
    send \"add_label 6000 PORTB\n\"
    expect -re \$prompt
    send \"add_label 6001 PORTA\n\"
    expect -re \$prompt
    send \"add_label 6002 DDRB\n\"
    expect -re \$prompt
    send \"add_label 6003 DDRA\n\"
    expect -re \$prompt

    for {set i 0} {\$i < [llength {${infinite_loops[*]}}]}  { incr i } {
        set  location  [lindex {${infinite_loops[*]}} \$i]
        send \"add_label \$location infiniteLoop\$i\n\"
        expect -re \$prompt
        send \"add_breakpoint infiniteLoop\$i\n\"
        expect -re \$prompt
    }

    foreach location {${method_locations[*]}} name {${method_names[*]}} {
        send \"add_label \$location \$name\n\"
        expect -re \$prompt
    }

    send \"fill 0000:7fff 0\n\"
    expect -re \$prompt
    log_user 1

    send \"show_labels\n\"
    expect -re \$prompt

    interact
"
