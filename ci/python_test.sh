#!/usr/bin/env bash

set -euxo pipefail

VENV="${PWD}/build/venv-python-test/"

if [[ ! -d "$VENV" ]]; then
    python3 -mvenv "$VENV"
fi

set +u  # ignore missing variables in activation script
source "$VENV/bin/activate"
set -u

$VENV/bin/pip -v install pytest
$VENV/bin/pip -v install --force .

# $VENV/bin/pytest
