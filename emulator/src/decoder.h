#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "state.h"

/*
    Creating bitfields for all the different types of instruction
*/

#pragma pack(push, 1)
typedef struct {
    uint32_t rd : 5;
    uint32_t operand : 18;
    uint32_t opi : 3;
    uint32_t : 3;
    uint32_t opc : 2;
    uint32_t sf : 1;
} DPImmInstruction_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t rd : 5;
    uint32_t rn : 5;
    uint32_t operand : 6;
    uint32_t rm : 5;
    uint32_t opr : 4;
    uint32_t : 3;
    uint32_t m : 1;
    uint32_t opc : 2;
    uint32_t sf : 1;
} DPRegInstruction_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t rt : 5;
    uint32_t xn : 5;
    uint32_t offset : 12;
    uint32_t l : 1;
    uint32_t : 7;
    uint32_t sf : 1;
    uint32_t : 1;
} SDTInstruction_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t rt : 5;
    uint32_t simm19 : 19;
    uint32_t : 6;
    uint32_t sf : 1;
    uint32_t : 1;
} loadLitInstruction_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t operand : 26;
    uint32_t : 4;
    uint32_t type : 2;
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

typedef struct {
    int32_t offset : 26;
} branchUnconditional_t;

typedef struct {
    uint32_t : 5;
    uint32_t xn : 5;
} branchRegister_t;

typedef struct {
    uint32_t cond : 4;
    uint32_t : 1;
    int32_t offset : 19;
} branchCondition_t;

typedef union {
    uint32_t raw;
    branchUnconditional_t branchUnconditional;
    branchRegister_t branchRegister;
    branchCondition_t branchCondition;
} branchOperand_t;

extern void executeDPImm(processorState_t *state, DPImmInstruction_t dpimm);

bool decodeAndExecute(processorState_t *state, uint32_t rawInstruction);
