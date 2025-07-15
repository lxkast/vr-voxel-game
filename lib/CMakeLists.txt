add_library(logging STATIC
    logging/logging.c
)

target_include_directories(logging
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/logging
)