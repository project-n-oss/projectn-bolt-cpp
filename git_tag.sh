#!/usr/bin/env bash

VERSION=`awk '/OWN_VERSION/ {$2; sub(/\)$/, "", $2); print $2; exit}' CMakeLists.txt` 

git tag -f -a VERSION
git push -f --tags