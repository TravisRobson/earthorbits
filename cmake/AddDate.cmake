# Add date library https://github.com/HowardHinnant/date
FetchContent_Declare(
    date
    URL https://github.com/HowardHinnant/date/archive/refs/tags/v3.0.1.zip
)

# set(BUILD_TZ_LIB CACHE BOOL ON)
set(USE_SYSTEM_TZ_DB CACHE BOOL ON)

FetchContent_MakeAvailable(date)
