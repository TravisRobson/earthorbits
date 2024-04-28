#!/usr/bin/env bash

set -o errexit

mkdir -p build-cmake-debug
scan-build cmake -S . -B build-cmake-debug \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DEOB_ENABLE_CLANG_TIDY=OFF \
    -DEOB_COMPILE_WARNINGS_AS_ERRORS=ON
scan-build --view cmake --build build-cmake-debug -j 6
./build-cmake-debug/tests/earthorbittests
# lldb -- ./build-cmake-debug/tests/earthorbittests

# mkdir -p build-cmake-release
# cmake -S . -B build-cmake-release \
#     -DCMAKE_BUILD_TYPE=Release \
#     -DCMAKE_EXPORT_COMPILE_COMMANDS=OFF \
#     -DEOB_ENABLE_CLANG_TIDY=OFF \
#     -DEOB_COMPILE_WARNINGS_AS_ERRORS=OFF \
#     -DBENCHMARK_ENABLE_LTO=true
# cmake --build build-cmake-release -j 6
# ./build-cmake-release/tests/benchmarksearthorbit
