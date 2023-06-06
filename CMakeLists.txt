cmake_minimum_required(VERSION 3.20)
project(chip8 VERSION 0.0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" FORCE)
message(STATUS "Installing glfw via submodule")
add_subdirectory(external/glfw)

# fmt
message(STATUS "Installing fmt via submodules")
add_subdirectory(external/fmt)

# glad
message(STATUS "Installing glad")
add_subdirectory(external/glad)


add_executable(chip8 src/main.cpp src/window.cpp src/renderer.cpp
    src/asset.cpp src/chip8.cpp)

target_link_libraries(chip8 glfw fmt Glad)

target_compile_options(chip8 PRIVATE -Wall -Wextra)
