#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "state.h"

/*
    Creating bitfields for all the different types of instruction
*/

#pragma pack(push, 1)
typedef struct {
    uint8_t rd : 5;
    uint32_t operand : 18;
    uint8_t opi : 3;
    uint8_t : 3;
    uint8_t opc : 2;
    uint8_t sf : 1;
} DPImmInstruction_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint8_t rd : 5;
    uint8_t rn : 5;
    uint8_t operand : 6;
    uint8_t rm : 5;
    uint8_t opr : 4;
    uint8_t : 3;
    uint8_t m : 1;
    uint8_t opc : 2;
    uint8_t sf : 1;
} DPRegInstruction_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint8_t rt : 5;
    uint8_t xn : 5;
    uint16_t offset : 12;
    uint8_t l : 1;
    uint8_t : 7;
    uint8_t sf : 1;
    uint8_t : 1;
} SDTInstruction_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint8_t rt : 5;
    uint32_t simm19 : 19;
    uint8_t : 6;
    uint8_t sf : 1;
    uint8_t : 1;
} loadLitInstruction_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t operand : 26;
    uint8_t : 4;
    uint8_t type : 2;
} branchInstruction_t;
#pragma pack(pop)

typedef union {
    uint32_t raw;
    DPImmInstruction_t dpimm;
    DPRegInstruction_t dpreg;
    SDTInstruction_t sdt;
    loadLitInstruction_t load;
    branchInstruction_t branch;
} instruction_u;

extern void executeDPImm(processorState_t *state, DPImmInstruction_t dpimm);

bool decodeAndExecute(processorState_t *state, uint32_t rawInstruction);
