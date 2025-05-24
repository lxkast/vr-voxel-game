file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_executable(emulate
    ${SRC_FILES}
)

target_link_libraries(emulate
    PRIVATE logging futil
)