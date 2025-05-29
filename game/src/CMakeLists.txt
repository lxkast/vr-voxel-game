file(GLOB_RECURSE SRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

add_subdirectory(hardware)
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/hardware/test.c")
add_executable(extension
        ${SRC_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE logging)

file(
        COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
