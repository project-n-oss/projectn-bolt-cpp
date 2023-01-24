#!/usr/bin/env bash
echo "Input aws-sdk-cpp release tag to use: "
read TAG
echo "Using tag=$TAG"


echo "Checking out tag..."
cd aws-sdk-cpp
git checkout $TAG
cd ..
