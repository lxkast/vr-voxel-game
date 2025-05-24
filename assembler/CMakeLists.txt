file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_executable(assembler
    ${SRC_FILES}
)

target_link_libraries(assembler
    PRIVATE logging
)