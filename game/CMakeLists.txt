cmake_minimum_required(VERSION 3.25)
project(game C)

set(CMAKE_C_STANDARD 17)

find_package(OpenGL)

add_subdirectory(src)
add_subdirectory(external)

set(PARENT_SRC "${CMAKE_SOURCE_DIR}/..")
set(PARENT_BUILD "${CMAKE_BINARY_DIR}/parent")

file(
        COPY ${CMAKE_CURRENT_SOURCE_DIR}/textures
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/src
)