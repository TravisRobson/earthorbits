# https://github.com/google/benchmark
FetchContent_Declare(
    googlebenchmark
    URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
)

# SET(HAVE_STD_REGEX CACHE BOOL ON)
# SET(RUN_HAVE_STD_REGEX CACHE BOOL ON)
set(BENCHMARK_ENABLE_TESTING CACHE BOOL OFF)

FetchContent_MakeAvailable(googlebenchmark)
