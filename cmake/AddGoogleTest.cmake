# GoogleTest https://google.github.io/googletest/quickstart-cmake.html
# https://github.com/google/googletest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)