#pragma once

#include <stdint.h>

#include "state.h"

extern uint8_t  read_mem8(processorState_t* state, uint32_t address);
extern uint16_t read_mem16(processorState_t* state, uint32_t address);
extern uint32_t read_mem32(processorState_t* state, uint32_t address);
extern uint64_t read_mem64(processorState_t* state, uint32_t address);

extern void write_mem8(processorState_t* state, uint32_t address, uint8_t data);
extern void write_mem16(processorState_t* state, uint32_t address, uint16_t data);
extern void write_mem32(processorState_t* state, uint32_t address, uint32_t data);
extern void write_mem64(processorState_t* state, uint32_t address, uint64_t data);
