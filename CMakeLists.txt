cmake_minimum_required(VERSION 4.0)

project(4004emu VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main src/main.cpp src/mnemonics.cpp)
