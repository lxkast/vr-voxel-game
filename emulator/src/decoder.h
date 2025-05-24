#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "state.h"

/*
    Creating bitfields for all the different types of instruction
*/

typedef struct {
    uint8_t sf : 1;
    uint8_t opc : 2;
    uint8_t : 3;
    uint8_t opi : 3;
    uint32_t operand : 18;
    uint8_t rd : 5;
} DPImmInstruction_t;

typedef struct {
    uint8_t sf : 1;
    uint8_t opc : 2;
    uint8_t m : 1;
    uint8_t : 3;
    uint8_t opr : 4;
    uint8_t rm : 5;
    uint8_t operand : 6;
    uint8_t rn : 5;
    uint8_t rd : 5;
} DPRegInstruction_t;

typedef struct {
    uint8_t : 1;
    uint8_t sf : 1;
    uint8_t : 7;
    uint8_t l : 1;
    uint16_t offset : 12;
    uint8_t xn : 5;
    uint8_t rt : 5;
} SDTInstruction_t;

typedef struct {
    uint8_t : 1;
    uint8_t sf : 1;
    uint8_t : 6;
    uint32_t simm19 : 19;
    uint8_t rt : 5;
} loadLitInstruction_t;

typedef struct {
    uint8_t type : 2;
    uint8_t : 4;
    uint32_t operand : 26;
} branchInstruction_t;

typedef union {
    uint32_t raw;
    DPImmInstruction_t dpimm;
    DPRegInstruction_t dpreg;
    SDTInstruction_t sdt;
    loadLitInstruction_t load;
    branchInstruction_t branch;
} instruction_u;

bool decodeAndExecute(processorState_t *state, const uint32_t rawInstruction);
