cmake_minimum_required(VERSION 3.25)
project(armv8 C)

set(CMAKE_C_STANDARD 17)

add_compile_options(-Wall -Wextra -pedantic -g -lm)

add_subdirectory(assembler-emulator)
add_subdirectory(lib)
