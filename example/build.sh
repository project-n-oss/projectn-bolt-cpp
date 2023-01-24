#!/usr/bin/env bash
set -ex

BUILD_DIR="./build"

mkdir -p $BUILD_DIR
pushd $BUILD_DIR

cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . 

popd