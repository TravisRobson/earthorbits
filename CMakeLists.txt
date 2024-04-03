cmake_minimum_required(VERSION 3.25)

project(earthorbits VERSION 0.1.0 LANGUAGES CXX)

# # https://stackoverflow.com/a/42983782
# set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=*)

find_program(CLANG_TIDY clang-tidy)

add_library(earthorbits src/earthorbits.cpp)

# To build the target we need headers from both locations.
# Public headers live in include. Users of this library only need to
# know about the headers in the include/ folder. At build time however
# the header is at $(pwd)/include/, but after install it's at 
# ${CMAKE_INSTALL_PREFIX}/include/, hence the need to specify the
# BUILD_ and INSTALL_ interfaces
target_include_directories(earthorbits
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
 
# https://github.com/cpp-best-practices/cppbestpractices/blob/master/02-Use_the_Tools_Available.md#compilers
target_compile_options(earthorbits 
    PRIVATE 
        # -Werror 
        -Wall
        -Wcast-align
        -Wconversion
        -Wextra
        -Wformat=2
        -Wimplicit-fallthrough
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpedantic
        -Wshadow
        -Wsign-conversion
        -Wunused
)
target_compile_features(earthorbits PRIVATE cxx_std_20)

# https://cmake.org/pipermail/cmake/2018-December/068739.html
set_target_properties(earthorbits
  PROPERTIES
    CXX_CLANG_TIDY ${CLANG_TIDY})
