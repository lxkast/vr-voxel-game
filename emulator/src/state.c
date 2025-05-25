#include <logging.h>
#include <string.h>
#include "state.h"

/*
    checks if state is a non-null pointer
    and reg is a valid index into the register file
 */
static void checkValidStateAndGPRegister(processorState_t *state, const reg_t reg) {
    if (!state) {
        LOG_FATAL("Cannot access null state to access general purpose register");
    }
    if (reg > 31 || reg < 0) {
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

/*
    write all condition codes in PSTATE
*/
void write_pState(processorState_t *state, const pState_t value) {
    if (!state) {
        LOG_FATAL("Cannot access null state to write to PSTATE");
    }
    state->spRegisters.PSTATE = value;
}

void initState(processorState_t *state, const uint32_t *programInstructions, const uint32_t numInstructions) {
    state->spRegisters.PC = 0;
    state->spRegisters.SP = 0;

    state->spRegisters.PSTATE.C = false;
    state->spRegisters.PSTATE.N = false;
    state->spRegisters.PSTATE.V = false;
    state->spRegisters.PSTATE.Z = true;

    memset(state->gpRegisters.regs, 0, sizeof(state->gpRegisters.regs));
    memset(state->memory, 0, sizeof(state->memory));


    memcpy(state->memory, programInstructions, numInstructions * sizeof(uint32_t));
}
