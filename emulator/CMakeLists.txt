add_executable(emulator
    src/emulate.c
    src/decode.c
)

target_link_libraries(emulator
    PRIVATE logging add
)
