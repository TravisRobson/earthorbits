if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    message(STATUS "::EARTHORBITS:: Configuring 'AppleClang' compiler options")
    set_property(GLOBAL PROPERTY EARTHORBITS_PRIVATE_COMPILE_OPTIONS
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
        # # Disabled warnings
        # -Wno-c++98-compat
        # -Wno-c++98-compat-pedantic # To silence some fmt:: warnings
    )
else()
    message(FATAL_ERROR "::EARTHORBITS:: Unsupported compiler detected: ${CMAKE_CXX_COMPILER_ID}")
endif()