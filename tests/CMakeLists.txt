include(AddGoogleTest)

add_executable(earthorbittests main.cpp)
target_link_libraries(earthorbittests PUBLIC GTest::gtest_main earthorbits)
target_compile_options(earthorbittests PUBLIC 
    ${EARTHORBIT_PRIVATE_COMPILE_OPTIONS}
)
target_compile_features(earthorbittests PRIVATE cxx_std_20)
target_compile_definitions(earthorbittests PRIVATE ${EARTHORIBTS_PRIVATE_COMPILE_DEFINES})
target_include_directories(earthorbittests
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

if (EOB_ENABLE_CLANG_TIDY)
    # https://cmake.org/pipermail/cmake/2018-December/068739.html
    set_target_properties(earthorbittests
        PROPERTIES
            CXX_CLANG_TIDY ${CLANG_TIDY}
    )
endif()

include(GoogleTest)
gtest_discover_tests(earthorbittests)
