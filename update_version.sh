#!/usr/bin/env bash
echo "Input aws-sdk-cpp release tag to use: "
read TAG
echo "Using tag=$TAG"


echo "Checking out tag..."
cd aws-sdk-cpp
git checkout $TAG
cd ..

echo "Updating CMakeLists.txt project version"
perl -i -pe "s/^set\(OWN_VERSION.*/set\(OWN_VERSION $TAG\)/" CMakeLists.txt
