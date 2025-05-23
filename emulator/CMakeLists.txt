add_executable(emulator
    src/emulate.c
)

target_link_libraries(emulator
    PRIVATE add
)