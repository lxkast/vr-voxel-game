#include "memory.h"
#include "logging.h"

uint8_t data[MEMORY_SIZE] = {0};

uint8_t MEM_get8(uint32_t address) {
    if (address >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to access outside of memory");
    }
    return data[address];
}


// As x86 is little endian this is fine

uint16_t MEM_get16(uint32_t address) {
    if (address + 1 >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to access outside of memory");
    }
    return *(uint16_t*)&data[address];
}

uint32_t MEM_get32(uint32_t address) {
    if (address + 3 >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to access outside of memory");
    }
    return *(uint32_t*)&data[address]; 
}

uint64_t MEM_get64(uint32_t address) {
    if (address + 7 >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to cacess outside of memory");
    }
    return *(uint64_t*)&data[address];
}

void MEM_set8(uint32_t address, uint8_t value) {
    if (address >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to write outside of memory");
    }
    data[address] = value;
}

void MEM_set16(uint32_t address, uint16_t value) {
    if (address >= MEMORY_SIZE + 1) {
        LOG_FATAL("Attempt to write outside of memory");
    }

    *(uint16_t*)&data[address] = value;
}

void MEM_set32(uint32_t address, uint32_t value) {
    if (address >= MEMORY_SIZE + 3) {
        LOG_FATAL("Attempt to write outside of memory");
    }
    *(uint32_t*)&data[address] = value;
}

void MEM_set64(uint32_t address, uint64_t value) {
    if (address >= MEMORY_SIZE + 7) {
        LOG_FATAL("Attempt to write outside of memory");
    }
    *(uint64_t*)&data[address] = value;
}
