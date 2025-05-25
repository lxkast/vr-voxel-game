#pragma once

/*
    Structs that define the processor state,
    with functions to manipulate state.
*/

#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE_BITS 21
#define MEMORY_SIZE (1 << MEMORY_SIZE_BITS)
typedef uint8_t memory_t[MEMORY_SIZE];

typedef uint8_t reg_t;

typedef struct {
    uint64_t regs[31];
} gpRegisters_t;

typedef struct {
    bool N;
    bool Z;
    bool C;
    bool V;
} pState_t;

typedef struct {
    uint64_t PC;
    uint64_t SP; // don't need to implement this.
    pState_t PSTATE;
} spRegisters_t;

typedef struct {
    gpRegisters_t gpRegisters;
    spRegisters_t spRegisters;
    memory_t memory;
} processorState_t;

uint32_t read_gpReg32(processorState_t *state, reg_t reg);
void write_gpReg32(processorState_t *state, reg_t reg, uint32_t value);

uint64_t read_gpReg64(processorState_t *state, reg_t reg);
void write_gpReg64(processorState_t *state, reg_t reg, uint64_t value);

uint64_t read_PC(processorState_t *state);
void write_PC(processorState_t *state, uint64_t value);

uint64_t read_SP(processorState_t *state);
void write_SP(processorState_t *state, uint64_t value);

uint64_t read_ZR(processorState_t *state);
void write_ZR(processorState_t *state, uint64_t value);

pState_t read_pState(processorState_t *state);
void write_pState(processorState_t *state, pState_t value);

void initState(processorState_t *state, const uint32_t *programInstructions, uint32_t numInstructions);
