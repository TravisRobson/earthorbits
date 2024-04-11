cmake_minimum_required(VERSION 3.25)

project(earthorbits VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(EOB_ENABLE_CLANG_TIDY "Enable clang-tidy checks when compiling" OFF)
# Below used so that local develop isn't made more annoying, but 
# still strong checks are enforced by CI
option(EOB_COMPILE_WARNINGS_AS_ERRORS "Make compiler warnings errors" OFF)
option(EOB_COMPILE_SANITIZERS "Compile with clang sanitizers" OFF)

message(STATUS "::EARTHORBITS:: EOB_ENABLE_CLANG_TIDY=${EOB_ENABLE_CLANG_TIDY}")
message(STATUS "::EARTHORBITS:: EOB_COMPILE_WARNINGS_AS_ERRORS=${EOB_COMPILE_WARNINGS_AS_ERRORS}")
message(STATUS "::EARTHORBITS:: EOB_COMPILE_SANITIZERS=${EOB_COMPILE_SANITIZERS}")

if (EOB_ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY clang-tidy)
    message(STATUS "FOUND CLANG_TIDY=${CLANG_TIDY}")
endif()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(FetchContent)
include(EarthorbitsOptions)

enable_testing() # Has to be in this top-level CMake

add_subdirectory(src)
# TODO make adding this subdirectory optional
add_subdirectory(tests)
