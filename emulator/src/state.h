#pragma once

/*
    Structs that define the processor state,
    with functions to manipulate state.
*/

#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE_BITS 21
#define MEMORY_SIZE (1 << MEMORY_SIZE_BITS)
#define MEMORY_OFFSET_BITS 10
#define MEMORY_LINE_SIZE (1 << MEMORY_OFFSET_BITS)
#define MEMORY_OFFSET_MASK (MEMORY_LINE_SIZE - 1)
#define MEMORY_INDEX_BITS (MEMORY_SIZE_BITS - MEMORY_OFFSET_BITS)
#define MEMORY_MAX_INDEX (1 << MEMORY_INDEX_BITS)
#define MEMORY_INDEX_MASK ~MEMORY_OFFSET_MASK

typedef uint8_t *memory_t[MEMORY_MAX_INDEX];
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
void increment_PC(processorState_t *state, int64_t amount);

uint64_t read_SP(processorState_t *state);
void write_SP(processorState_t *state, uint64_t value);

uint64_t read_ZR(processorState_t *state);
void write_ZR(processorState_t *state, uint64_t value);

pState_t read_pState(processorState_t *state);
void write_pState(processorState_t *state, pState_t value);


/*
 * The functions below either write to the appropriate register, or if reg == 0b11111 write to the ZR register
 */
extern void write_reg64z(processorState_t *state, reg_t reg, uint64_t value);
extern void write_reg32z(processorState_t *state, reg_t reg, uint32_t value);
extern uint64_t read_reg64z(processorState_t *state, reg_t reg);
extern uint32_t read_reg32z(processorState_t *state, reg_t reg);


extern uint8_t  read_mem8(processorState_t* state, uint64_t address);
extern uint16_t read_mem16(processorState_t* state, uint64_t address);
extern uint32_t read_mem32(processorState_t* state, uint64_t address);
extern uint64_t read_mem64(processorState_t* state, uint64_t address);

extern void write_mem8(processorState_t* state, uint64_t address, uint8_t data);
extern void write_mem16(processorState_t* state, uint64_t address, uint16_t data);
extern void write_mem32(processorState_t* state, uint64_t address, uint32_t data);
extern void write_mem64(processorState_t* state, uint64_t address, uint64_t data);

extern void write_memory(processorState_t *state, FILE *file);

void initState(processorState_t *state, const uint32_t *programInstructions, uint32_t numInstructions);
void freeState(processorState_t *state);
