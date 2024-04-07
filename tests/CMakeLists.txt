include(AddGoogleTest)

get_property(earthorbit_private_compile_options GLOBAL PROPERTY 
    EARTHORBIT_PRIVATE_COMPILE_OPTIONS)

add_executable(tests main.cpp)
target_link_libraries(tests PUBLIC GTest::gtest_main earthorbits)
target_compile_options(tests PUBLIC 
    ${orbit_private_compile_options}
    # -Wno-global-constructors
    # -Wno-c++98-compat-pedantic
)
target_include_directories(tests
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

if (EOB_ENABLE_CLANG_TIDY)
    # https://cmake.org/pipermail/cmake/2018-December/068739.html
    set_target_properties(tests
        PROPERTIES
            CXX_CLANG_TIDY ${CLANG_TIDY}
    )
endif()

include(GoogleTest)
gtest_discover_tests(tests)