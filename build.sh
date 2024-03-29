#!/usr/bin/env bash

build_type_flag='Release'
target_flag='all'


while getopts 'dri' flag; do
    case "${flag}" in
        d) build_type_flag='Debug' ;;
        r) build_type_flag='Release' ;;
        i) target_flag="install" ;; 
    esac
done


set -ex

BUILD_DIR="./build"

mkdir -p $BUILD_DIR
pushd $BUILD_DIR

cmake -DCMAKE_BUILD_TYPE=$build_type_flag ..
cmake --build . --target $target_flag --config $build_type_flag

popd