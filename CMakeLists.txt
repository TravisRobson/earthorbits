cmake_minimum_required(VERSION 3.25)

project(earthorbits VERSION 0.1.0 LANGUAGES CXX)

find_program(CLANG_TIDY clang-tidy)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(FetchContent)
include(EarthorbitsOptions)

enable_testing() # Has to be in this top-level CMake

add_subdirectory(src)
# TODO make adding this subdirectory optional
add_subdirectory(tests)
