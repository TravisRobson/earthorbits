include(AddGoogleTest)
include(AddDate)

add_executable(earthorbittests main.cpp)
target_link_libraries(earthorbittests PUBLIC GTest::gtest_main earthorbits date fmt::fmt)
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
# Prevent clang-tidy from checking date headers
# https://davy.ai/ignore-3rd-party-headers-from-clang-tidy-in-cmake/
target_include_directories(earthorbittests 
    SYSTEM PUBLIC 
        ${CMAKE_BINARY_DIR}/_deps/date-src/include/
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

# It doesn't make sense to benchmark in debug mode
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    include(AddGoogleBenchmark)

    add_executable(benchmarksearthorbit benchmarks.cpp)
    target_link_libraries(benchmarksearthorbit PUBLIC benchmark::benchmark earthorbits date)
    target_compile_options(benchmarksearthorbit PUBLIC 
        ${EARTHORBIT_PRIVATE_COMPILE_OPTIONS}
    )
    target_compile_features(benchmarksearthorbit PRIVATE cxx_std_20)
    target_compile_definitions(benchmarksearthorbit PRIVATE ${EARTHORIBTS_PRIVATE_COMPILE_DEFINES})
    target_include_directories(benchmarksearthorbit
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    )
    # Prevent clang-tidy from checking date headers
    # https://davy.ai/ignore-3rd-party-headers-from-clang-tidy-in-cmake/
    target_include_directories(benchmarksearthorbit 
        SYSTEM PUBLIC 
            ${CMAKE_BINARY_DIR}/_deps/date-src/include/
    )

    if (EOB_ENABLE_CLANG_TIDY)
        # https://cmake.org/pipermail/cmake/2018-December/068739.html
        set_target_properties(benchmarksearthorbit
            PROPERTIES
                CXX_CLANG_TIDY ${CLANG_TIDY}
        )
    endif()
endif()
