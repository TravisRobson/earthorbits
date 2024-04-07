
add_library(earthorbits earthorbits.cpp)


# To build the target we need headers from both locations.
# Public headers live in include. Users of this library only need to
# know about the headers in the include/ folder. At build time however
# the header is at $(pwd)/include/, but after install it's at 
# ${CMAKE_INSTALL_PREFIX}/include/, hence the need to specify the
# BUILD_ and INSTALL_ interfaces
target_include_directories(earthorbits
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Prevent clang-tidy from checking fmt headers
# https://davy.ai/ignore-3rd-party-headers-from-clang-tidy-in-cmake/
target_include_directories(earthorbits SYSTEM PUBLIC ${CMAKE_BINARY_DIR}/_deps/fmt-src/include/)

get_property(earthorbit_private_compile_options GLOBAL PROPERTY 
    EARTHORBIT_PRIVATE_COMPILE_OPTIONS)
 
# https://github.com/cpp-best-practices/cppbestpractices/blob/master/02-Use_the_Tools_Available.md#compilers
target_compile_options(earthorbits 
    PRIVATE 
        ${earthorbit_private_compile_options}
)
target_compile_features(earthorbits PRIVATE cxx_std_20)
include(AddFmt)
target_link_libraries(earthorbits PRIVATE fmt::fmt)

if (EOB_ENABLE_CLANG_TIDY)
    # https://cmake.org/pipermail/cmake/2018-December/068739.html
    set_target_properties(earthorbits
        PROPERTIES
            CXX_CLANG_TIDY ${CLANG_TIDY}
    )
endif()