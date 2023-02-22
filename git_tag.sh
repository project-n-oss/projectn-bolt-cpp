#!/usr/bin/env bash

VERSION=`awk '/OWN_VERSION/ {$2; sub(/\)$/, "", $2); print $2; exit}' CMakeLists.txt` 
MSG=$(git log -1 --pretty=%B)
git tag -f -a $VERSION -m "$MSG"
git push -f --tags