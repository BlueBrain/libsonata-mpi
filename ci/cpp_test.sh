#!/usr/bin/env bash

# This file runs the C++ tests, as well as compiling the code with warnings on
# so that errors should be caught quicker

set -euxo pipefail

BUILD_DIR=build/build-cpp-test
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

cmake -DCMAKE_BUILD_TYPE=Release                                             \
      -DSONATA_MPI_FROM_SUBMODULES=ON                                        \
      -B $BUILD_DIR                                                          \
      .

cmake --build "$BUILD_DIR" --parallel
ctest --test-dir "$BUILD_DIR"
