#!/usr/bin/env bash

mkdir build
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=OFF \
    -DEOB_ENABLE_CLANG_TIDY=OFF \
    -DEOB_COMPILE_WARNINGS_AS_ERRORS=ON
cmake --build build
