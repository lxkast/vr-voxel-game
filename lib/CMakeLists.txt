add_library(add STATIC
    add/add.c
)

target_include_directories(add 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/add 
)