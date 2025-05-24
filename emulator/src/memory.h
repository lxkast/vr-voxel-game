#pragma once

#include<stdint.h>

#define MEMORY_SIZE_BITS 21
#define MEMORY_SIZE (1 << MEMORY_SIZE_BITS)

extern uint8_t  MEM_getByte(uint32_t address);
extern uint16_t MEM_getShort(uint32_t address);
extern uint32_t MEM_getInt(uint32_t address);
extern uint64_t MEM_getLong(uint32_t address);

extern void MEM_setByte(uint32_t address, uint8_t data);
extern void MEM_setShort(uint32_t address, uint16_t data);
extern void MEM_setInt(uint32_t address, uint16_t data);

void MEM_intialise(void);

// TODO give this decent parameters
void MEM_prettyPrint(void);
void MEM_clear(void);

