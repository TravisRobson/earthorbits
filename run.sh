#!/usr/bin/env bash

mkdir build
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DEOB_ENABLE_CLANG_TIDY=ON
cmake --build build
