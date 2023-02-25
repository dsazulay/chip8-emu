cmake_minimum_required(VERSION 3.20)
project(chip8 VERSION 0.0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#fmt
message(STATUS "Installing fmt via submodules")
add_subdirectory(external/fmt)

add_executable(chip8 src/main.cpp)

target_link_libraries(chip8 fmt)

target_compile_options(chip8 PRIVATE -Wall -Wextra)
