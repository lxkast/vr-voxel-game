file(GLOB_RECURSE SRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

if(OPENGL_FOUND)
    set(TO_EXCLUDE "gles2.c")
else()
    set(TO_EXCLUDE "gl.c")
endif()

list(FILTER SRC_FILES EXCLUDE REGEX
        "${CMAKE_CURRENT_SOURCE_DIR}/${TO_EXCLUDE}"
)

add_executable(extension
        ${SRC_FILES}
)

file(
        COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)