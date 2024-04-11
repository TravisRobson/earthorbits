if (EOB_COMPILE_SANITIZERS)
    # https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug.html
    set(EARTHORBITS_PRIVATE_COMPILE_DEFINES
        -D_GLIBCXX_DEBUG
        -D_GLIBCXX_DEBUG_PEDANTIC
        -D_GLIBCXX_SANITIZE_VECTOR
        -D_GLIBCXX_ASSERTIONS
    )
endif()
