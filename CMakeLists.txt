cmake_minimum_required(VERSION 4.0)

project(i4004emu VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG        12.1.0)
FetchContent_MakeAvailable(fmt)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(i4004emu src/main.cpp src/mnemonics.cpp src/cpu.cpp)
target_link_libraries(i4004emu fmt::fmt)
