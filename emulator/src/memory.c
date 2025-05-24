#include "memory.h"
#include "logging.h"

#define NOT_NULL(ptr) { if (!ptr) LOG_FATAL("NULL pointer"); }

uint8_t read_mem8(processorState_t* state, uint32_t address) {
    NOT_NULL(state);
    if (address >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to access outside of memory");
    }
    return state->memory[address];
}


// As x86 is little endian this is fine

uint16_t read_mem16(processorState_t* state, uint32_t address) {
    NOT_NULL(state);
    if (address + 1 >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to access outside of memory");
    }
    return *(uint16_t*)&state->memory[address];
}

uint32_t read_mem32(processorState_t* state, uint32_t address) {
    NOT_NULL(state);
    if (address + 3 >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to access outside of memory");
    }
    return *(uint32_t*)&state->memory[address];
}

uint64_t read_mem64(processorState_t* state, uint32_t address) {
    NOT_NULL(state);
    if (address + 7 >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to cacess outside of memory");
    }
    return *(uint64_t*)&state->memory[address];
}

void write_mem8(processorState_t* state, uint32_t address, uint8_t value) {
    NOT_NULL(state);
    if (address >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to write outside of memory");
    }
    state->memory[address] = value;
}

void write_mem16(processorState_t* state, uint32_t address, uint16_t value) {
    NOT_NULL(state);
    if (address >= MEMORY_SIZE + 1) {
        LOG_FATAL("Attempt to write outside of memory");
    }

    *(uint16_t*)&state->memory[address] = value;
}

void write_mem32(processorState_t* state, uint32_t address, uint32_t value) {
    NOT_NULL(state);
    if (address >= MEMORY_SIZE + 3) {
        LOG_FATAL("Attempt to write outside of memory");
    }
    *(uint32_t*)&state->memory[address] = value;
}

void write_mem64(processorState_t* state, uint32_t address, uint64_t value) {
    NOT_NULL(state);
    if (address >= MEMORY_SIZE + 7) {
        LOG_FATAL("Attempt to write outside of memory");
    }
    *(uint64_t*)&state->memory[address] = value;
}
