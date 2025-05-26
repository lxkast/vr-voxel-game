#include <logging.h>
#include <string.h>
#include "state.h"

#include "memory.h"
#define NOT_NULL(ptr) { if (!ptr) LOG_FATAL("NULL pointer"); }

/*
    checks if state is a non-null pointer
    and reg is a valid index into the register file
 */
static void checkValidStateAndGPRegister(processorState_t *state, const reg_t reg) {
    if (!state) {
        LOG_FATAL("Cannot access null state to access general purpose register");
    }
    if (reg >= 31) {
        LOG_FATAL("Cannot access invalid general purpose register");
    }
}

/*
    read 32-bit value from a general-purpose register
*/
uint32_t read_gpReg32(processorState_t *state, const reg_t reg) {
    checkValidStateAndGPRegister(state, reg);
    return state->gpRegisters.regs[reg];
}

/*
    write 32-bit value to a general-purpose register
 */
void write_gpReg32(processorState_t *state, const reg_t reg, const uint32_t value) {
    checkValidStateAndGPRegister(state, reg);
    state->gpRegisters.regs[reg] = value;
}

/*
    read 64-bit value from a general-purpose register
*/
uint64_t read_gpReg64(processorState_t *state, const reg_t reg) {
    checkValidStateAndGPRegister(state, reg);
    return state->gpRegisters.regs[reg];
}

/*
    write 64-bit value to a general-purpose register
*/
void write_gpReg64(processorState_t *state, const reg_t reg, const uint64_t value) {
    checkValidStateAndGPRegister(state, reg);
    state->gpRegisters.regs[reg] = value;
}

/*
    read 64-bit value from PC register
*/
uint64_t read_PC(processorState_t *state) {
    if (!state) {
        LOG_FATAL("Cannot access null state to read PC register");
    }
    return state->spRegisters.PC;
}

/*
    write 64-bit value to PC register
*/
void write_PC(processorState_t *state, const uint64_t value) {
    if (!state) {
        LOG_FATAL("Cannot access null state to write to PC register");
    }
    state->spRegisters.PC = value;
}

/*
    increments PC register by amount
*/
void increment_PC(processorState_t *state, int64_t amount) {
    if (!state) {
        LOG_FATAL("Cannot access null state to increment PC register");
    }
    state->spRegisters.PC += amount;
}

/*
    read 64-bit value from SP register
*/
uint64_t read_SP(processorState_t *state) {
    if (!state) {
        LOG_FATAL("Cannot access null state to read SP register");
    }
    return state->spRegisters.SP;
}

/*
    write 64-bit value to SP register
*/
void write_SP(processorState_t *state, const uint64_t value) {
    if (!state) {
        LOG_FATAL("Cannot access null state to write to SP register");
    }
    state->spRegisters.SP = value;
}

/*
    read 64-bit value from ZR register
*/
uint64_t read_ZR(processorState_t *state) {
    if (!state) {
        LOG_FATAL("Cannot access null state to read ZR register");
    }
    return 0;
}

/*
    write 64-bit value to ZR register
*/
void write_ZR(processorState_t *state, uint64_t value) {
    if (!state) {
        LOG_FATAL("Cannot access null state to write to ZR register");
    }
}

/*
    read all condition codes in PSTATE
*/
pState_t read_pState(processorState_t *state) {
    if (!state) {
        LOG_FATAL("Cannot access null state to read PSTATE");
    }
    return state->spRegisters.PSTATE;
}

void write_reg64z(processorState_t *state, const reg_t reg, const uint64_t value) {
    if (reg == 0x1F) {
        write_ZR(state, value);
    } else {
        write_gpReg64(state, reg,  value);
    }
}
void write_reg32z(processorState_t *state, const reg_t reg, const uint32_t value) {
    if (reg == 0x1F) {
        write_ZR(state, value);
    } else {
        write_gpReg32(state, reg,  value);
    }
}
uint64_t read_reg64z(processorState_t *state, const reg_t reg) {
    if (reg == 0x1F) {
        return read_ZR(state);
    } else {
        return read_gpReg64(state, reg);
    }
}
uint32_t read_reg32z(processorState_t *state, const reg_t reg) {
    if (reg == 0x1F) {
        return read_ZR(state);
    } else {
        return read_gpReg32(state, reg);
    }
}


/*
    write all condition codes in PSTATE
*/
void write_pState(processorState_t *state, const pState_t value) {
    if (!state) {
        LOG_FATAL("Cannot access null state to write to PSTATE");
    }
    state->spRegisters.PSTATE = value;
}


uint8_t* getLine(processorState_t *state, uint64_t address) {
    uint64_t lineIndex = (address & MEMORY_INDEX_MASK) >> MEMORY_OFFSET_BITS;
    if (!state->memory[lineIndex]) {
        state->memory[lineIndex] = malloc(MEMORY_LINE_SIZE);
        memset(state->memory[lineIndex], 0, MEMORY_LINE_SIZE);
    }
    return state->memory[lineIndex];
}

#define VALUE_OF(state, address) (getLine(state, address)[address & MEMORY_OFFSET_MASK])

uint8_t read_mem8(processorState_t* state, uint64_t address) {
    NOT_NULL(state);
    if (address >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to access outside of memory");
    }
    return VALUE_OF(state, address);
}

uint16_t read_mem16Unaligned(processorState_t* state, uint64_t address) {
    return read_mem8(state, address) + ((uint16_t) read_mem8(state, address + 1) << 8);
}

uint16_t read_mem16(processorState_t* state, uint64_t address) {
    NOT_NULL(state);
    if (address + 1 >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to access outside of memory");
    }
    if (address & 0x1) {
        return read_mem16Unaligned(state, address);
    }
    return *(uint16_t*)&VALUE_OF(state, address);
}

uint32_t read_mem32Unaligned(processorState_t* state, uint64_t address) {
    return read_mem16(state, address) + ((uint32_t) read_mem16(state, address + 2) << 16);
}

uint32_t read_mem32(processorState_t* state, uint64_t address) {
    NOT_NULL(state);
    if (address + 3 >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to access outside of memory");
    }
    if (address & 0x3) {
        return read_mem32Unaligned(state, address);
    }
    return *(uint32_t*)&VALUE_OF(state, address);
}

uint64_t read_mem64Unaligned(processorState_t* state, uint64_t address) {
    return read_mem32(state, address) + ((uint64_t) read_mem32(state, address + 4) << 32);
}

uint64_t read_mem64(processorState_t* state, uint64_t address) {
    NOT_NULL(state);
    if (address + 7 >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to cacess outside of memory");
    }
    if (address & 0x7) {
        return read_mem64Unaligned(state, address);
    }
    return *(uint64_t*)&VALUE_OF(state, address);
}


void write_mem8(processorState_t* state, uint64_t address, uint8_t value) {
    NOT_NULL(state);
    if (address >= MEMORY_SIZE) {
        LOG_FATAL("Attempt to write outside of memory");
    }
    VALUE_OF(state, address) = value;
}

void write_mem16Unaligned(processorState_t* state, uint64_t address, uint16_t value) {
    write_mem8(state, address, value & 0xFF);
    write_mem8(state, address + 1, value >> 8);
}

void write_mem16(processorState_t* state, uint64_t address, uint16_t value) {
    NOT_NULL(state);
    if (address >= MEMORY_SIZE + 1) {
        LOG_FATAL("Attempt to write outside of memory");
    }
    if (address & 0x1) {
        write_mem16Unaligned(state, address, value);
    } else {
        *(uint16_t*)&VALUE_OF(state, address) = value;
    }
}

void write_mem32Unaligned(processorState_t* state, uint64_t address, uint32_t value) {
    write_mem16(state, address, value & 0xFFFF);
    write_mem16(state, address + 2, value >> 16);
}

void write_mem32(processorState_t* state, uint64_t address, uint32_t value) {
    NOT_NULL(state);
    if (address >= MEMORY_SIZE + 3) {
        LOG_FATAL("Attempt to write outside of memory");
    }
    if (address & 0x3) {
        write_mem32Unaligned(state, address, value);
    } else {
        *(uint32_t*)&VALUE_OF(state, address) = value;
    }
}

void write_mem64Unaligned(processorState_t* state, uint64_t address, uint64_t value) {
    write_mem32(state, address, value & 0xFFFFFFFF);
    write_mem32(state, address + 4, value >> 32);
}

void write_mem64(processorState_t* state, uint64_t address, uint64_t value) {
    NOT_NULL(state);
    if (address >= MEMORY_SIZE + 7) {
        LOG_FATAL("Attempt to write outside of memory");
    }
    if (address & 0x7) {
        write_mem64Unaligned(state, address, value);
    } else {
        *(uint64_t*)&VALUE_OF(state, address) = value;
    }
}

void initialise_memory(processorState_t *state, const uint32_t *programInstructions, const uint32_t numInstructions) {
    for (uint32_t i = 0; i < numInstructions; i++) {
        write_mem32(state, i*4, programInstructions[i]);
    }
}

void write_memory(processorState_t *state, FILE *file) {
    for (uint32_t i = 0; i < MEMORY_MAX_INDEX; i++) {
        if (state->memory[i]) {
            for (uint32_t j = 0; j < MEMORY_LINE_SIZE; j+=4) {
                uint32_t value = read_mem32(state, (i << MEMORY_OFFSET_BITS) + j);
                if (value) {
                    fprintf(file, "0x%08X : %08x\n", (i << MEMORY_OFFSET_BITS) + j, value);
                }
            }
        }
    }
}

void free_memory(processorState_t *state) {
    for (uint32_t i = 0; i < MEMORY_MAX_INDEX; i++) {
        if (state->memory[i]) {
            free(state->memory[i]);
            state->memory[i] = NULL;
        }
    }
}

void initState(processorState_t *state, const uint32_t *programInstructions, const uint32_t numInstructions) {
    state->spRegisters.PC = 0;
    state->spRegisters.SP = 0;

    state->spRegisters.PSTATE.C = false;
    state->spRegisters.PSTATE.N = false;
    state->spRegisters.PSTATE.V = false;
    state->spRegisters.PSTATE.Z = true;

    memset(state->gpRegisters.regs, 0, sizeof(state->gpRegisters.regs));

    initialise_memory(state, programInstructions, numInstructions);
}

void freeState(processorState_t *state) {
    free_memory(state);
    free(state);
}
