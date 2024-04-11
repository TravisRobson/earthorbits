#!/usr/bin/env bash

mkdir build
scan-build cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DEOB_ENABLE_CLANG_TIDY=OFF \
    -DEOB_COMPILE_WARNINGS_AS_ERRORS=ON
scan-build --view cmake --build build -j 6
