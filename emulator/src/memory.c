#include "memory.h"


uint8_t *data;

void MEM_initialise(void) {
    *data = malloc(MEMORY_SIZE);
}

void MEM_getShort(uint8_t address) {
    if (address > MEMORY_SIZE) {
        LOG_fatal("out of memory");
    }
}
