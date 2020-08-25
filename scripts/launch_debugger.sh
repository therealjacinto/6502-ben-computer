#!/bin/bash
set -e

CONDA_PATH="/opt/conda"
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

while [ $# -gt 0 ]; do
    case $1 in
        "--verbose")
            verbose=true
            ;;
        "--ignore-segments")
            ignore_segments=true
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
if ! command -v conda &> /dev/null; then
    printf "ERROR: conda is not installed. Please install it before running "
    printf "this script. And install the conda environment by running the "
    printf "install_conda_env.sh script.\n"
    exit 1
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
    echo 
}

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
if [[ ${verbose} ]]; then
    printf "INFO: py65mon path found! (${PY65MON_PATH})\n"
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

# Look for debugger file with segment info
if [[ ! ${ignore_segments} ]]; then
    if [ -f ${FILE_PATH}-debug ] && [ $(stat -c %Y ${FILE_PATH}) == $(stat -c %Y ${FILE_PATH}-debug) ]; then
        if [[ ${verbose} ]]; then
            printf "INFO: rawseg debug file found! Using this file to add labels to "
            printf "monitor...\n"
        fi
        segment_table=$(cat ${FILE_PATH}-debug | awk -F "$(basename ${FILE_PATH})-debug." '{print $2}' | sed 's/\"//g')
        segment_names=($(echo "$segment_table" | awk -F " " '{ print $1 }'))
        segment_locations=($(echo "$segment_table" | awk -F " " '{ print $2 }' | awk -F "x" '{ print $2 }'))
        segment_lengths=($(echo "$segment_table" | awk -F " " '{ print $3 }' | awk -F "x" '{ print $2 }'))
    else
        printf "WARN: It looks like you don't have a debugger file for the binary "
        printf "you inputted. If you want segment tables added as labels to the "
        printf "binary, run:\n"
        printf "\t%s/buildmanually.sh --all --debug --save-output-files\n" "$(dirname ${SCRIPT})"
        read -p "Do you want to continue (press y to continue)?" -n 1 -r
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            printf "\nINFO: Exiting...\n"
            exit
        else
            printf "\n"
        fi
    fi
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

expect -c "
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

    foreach register {0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31} {
        send \"add_label \$register r\$register\n\"
        expect -re \$prompt
    }

    send \"add_label 32 sp\n\"
    expect -re \$prompt

    foreach register {0 1 2 3} {
        send \"add_label [expr 33 + 4 * \$register] btmp\$register\n\"
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

    foreach name {${segment_names[*]}} location {${segment_locations[*]}} {
        send \"add_label \$location \$name\n\"
        expect -re \$prompt
    }

    for {set i 0} {\$i < [llength {${jmp_address_machine_hex[*]}}]}  { incr i } {
        set  location  [lindex {${jmp_address_machine_hex[*]}} \$i]
        send \"add_label \$location infiniteLoop\$i\n\"
        expect -re \$prompt
        send \"add_breakpoint infiniteLoop\$i\n\"
        expect -re \$prompt
    }

    send \"fill 0000:7fff 0\n\"
    expect -re \$prompt

    send \"add_label 8000 romStart\n\"
    expect -re \$prompt
    send \"add_label ${START_ADDR_LE} start\n\"
    expect -re \$prompt
    send \"add_label ffff romEnd\n\"
    expect -re \$prompt
    send \"show_labels\n\"
    expect -re \$prompt

    interact
"
