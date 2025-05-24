#pragma once

/*
    Structs that define the processor state,
    with functions to manipulate state.
*/

#include <stdint.h>
#include <stdbool.h>

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
    uint64_t ZR;
    uint64_t PC;
    uint64_t SP; // don't need to implement this.
    pState_t PSTATE;
} spRegisters_t;

typedef struct {
    gpRegisters_t gpRegisters;
    spRegisters_t spRegisters;
} processorState_t;
