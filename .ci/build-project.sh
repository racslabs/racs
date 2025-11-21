#!/bin/bash

set -e

CMAKE_BUILD_ARGS=$1
BUILD_DIR=build

echo "---- build-project.sh ----"
echo "BUILD_DIR: $BUILD_DIR"
echo "CC: $CC"
echo "CXX: $CXX"
echo "CXXFLAGS: $CXXFLAGS"
echo "CMAKE_BUILD_ARGS: $CMAKE_BUILD_ARGS"
echo "--------------------------"

if [ ! -d "$BUILD_DIR" ] ; then
  mkdir "$BUILD_DIR"
fi

cmake -G Ninja "$CMAKE_BUILD_ARGS" -DCMAKE_BUILD_TYPE=Debug -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" --target racs -j 10