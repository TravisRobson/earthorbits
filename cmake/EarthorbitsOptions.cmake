if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    message(STATUS "::EARTHORBITS:: Configuring 'AppleClang' or 'Clang' compiler options")
    set(EARTHORBITS_PRIVATE_COMPILE_OPTIONS
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

    if (EOB_COMPILE_WARNINGS_AS_ERRORS)
        list(APPEND EARTHORBITS_PRIVATE_COMPILE_OPTIONS
            -Werror
        )
    endif()

    if (EOB_COMPILE_SANITIZERS)
        # https://clang.llvm.org/docs/AddressSanitizer.html
        # https://clang.llvm.org/docs/ThreadSafetyAnalysis.html
        list(APPEND EARTHORBITS_PRIVATE_COMPILE_OPTIONS
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
            -Wthread-safety
        )
    endif()

    message(STATUS "::EARTHORBITS:: ${EARTHORBITS_PRIVATE_COMPILE_OPTIONS}")
else()
    message(FATAL_ERROR "::EARTHORBITS:: Unsupported compiler detected: ${CMAKE_CXX_COMPILER_ID}")
endif()
