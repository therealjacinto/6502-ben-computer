#!/bin/bash
set -e
CONDA_PATH="/opt/conda"

# Get path of script
SCRIPTS_PATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
REPO_PATH="${SCRIPTS_PATH}/.."

cd ${REPO_PATH}
source ${CONDA_PATH}/etc/profile.d/conda.sh
conda deactivate
conda create -n 6502 python=3.6 -y
conda activate 6502
pip install -r requirements.txt
