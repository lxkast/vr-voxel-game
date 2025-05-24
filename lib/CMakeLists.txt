add_library(logging STATIC
    logging/logging.c
)
add_library(futil STATIC
    futil/futil.c
)
target_include_directories(logging
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/logging
)
target_include_directories(futil
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/futil
)