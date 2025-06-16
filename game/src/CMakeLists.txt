file(GLOB SRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)
if (BUILD_FOR_RPI)
    file(GLOB HARDWARE_SRC_FILES
      "${CMAKE_CURRENT_SOURCE_DIR}/hardware/*.c"
    )
    list(APPEND SRC_FILES ${HARDWARE_SRC_FILES})
    add_compile_definitions(BUILD_FOR_RPI)
    add_subdirectory(hardware)
endif()

list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/hardware/test.c")
add_executable(extension
        ${SRC_FILES}
        item.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE logging)

file(
        COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
