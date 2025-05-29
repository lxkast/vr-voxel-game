add_library(logging STATIC
    logging/logging.c
)
add_library(futil STATIC
    futil/futil.c
)

add_library(datastructs STATIC
        datastructs/datastructs.c
        datastructs/datastructs.h)

add_executable(datastructs_test
        datastructs/test.c
)

target_link_libraries(datastructs_test
        PRIVATE datastructs logging
)

target_include_directories(logging
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/logging
)
target_include_directories(futil
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/futil
)
target_include_directories(datastructs
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/datastructs
)