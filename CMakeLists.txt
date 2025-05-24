cmake_minimum_required(VERSION 3.28)
project(armv8 C)

set(CMAKE_C_STANDARD 17)

add_compile_options(-Wall -Wextra -pedantic)

add_subdirectory(assembler)
add_subdirectory(emulator)
add_subdirectory(lib)