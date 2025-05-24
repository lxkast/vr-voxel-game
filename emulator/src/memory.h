#pragma once

#include<stdint.h>

#define MEMORY_SIZE_BITS 21
#define MEMORY_SIZE (1 << MEMORY_SIZE_BITS)

extern uint8_t  MEM_get8(uint32_t address);
extern uint16_t MEM_get16(uint32_t address);
extern uint32_t MEM_get32(uint32_t address);
extern uint64_t MEM_get64(uint32_t address);

extern void MEM_set8(uint32_t address, uint8_t data);
extern void MEM_set16(uint32_t address, uint16_t data);
extern void MEM_set32(uint32_t address, uint32_t data);
extern void MEM_set64(uint32_t address, uint64_t data);
