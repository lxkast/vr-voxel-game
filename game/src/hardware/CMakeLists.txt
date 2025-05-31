file(GLOB_RECURSE SRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

add_executable(gyro-test
    ${SRC_FILES}
)
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test.c")
target_link_libraries(gyro-test PRIVATE logging m)
