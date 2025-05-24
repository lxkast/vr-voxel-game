cmake_minimum_required(VERSION 3.25)
project(extension C)

set(CMAKE_C_STANDARD 17)

find_package(OpenGL)

add_subdirectory(src)
add_subdirectory(external)
