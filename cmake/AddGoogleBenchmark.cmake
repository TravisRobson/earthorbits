# https://github.com/google/benchmark
FetchContent_Declare(
    googlebenchmark
    URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
)

set(BENCHMARK_ENABLE_TESTING OFF)

FetchContent_MakeAvailable(googlebenchmark)
