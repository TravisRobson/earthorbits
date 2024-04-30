# Add fmtlib string formatting library https://fmt.dev/latest/index.html
FetchContent_Declare(
    date
    URL https://github.com/HowardHinnant/date/archive/refs/tags/v3.0.1.zip
)

# set(BUILD_TZ_LIB CACHE BOOL ON)
set(BUILD_TZ_LIB ON)

set(USE_SYSTEM_TZ_DB CACHE BOOL ON)

FetchContent_MakeAvailable(date)
