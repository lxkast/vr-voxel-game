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
    uint32_t : 1;
    uint32_t u : 1;
    uint32_t : 5;
    uint32_t sf : 1;
    uint32_t : 1;
} SDTInstruction_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t rt : 5;
    int32_t simm19 : 19;
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

#pragma pack(push, 1)
typedef struct {
    int32_t offset : 26;
} branchUnconditional_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t : 5;
    uint32_t xn : 5;
} branchRegister_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t cond : 4;
    uint32_t : 1;
    int32_t offset : 19;
} branchCondition_t;
#pragma pack(pop)

typedef union {
    uint32_t raw;
    branchUnconditional_t branchUnconditional;
    branchRegister_t branchRegister;
    branchCondition_t branchCondition;
} branchOperand_t;

#pragma pack(push, 1)
typedef struct {
    uint32_t rn : 5;
    uint32_t imm12 : 12;
    uint32_t sh : 1;
} arithmeticOperand_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t imm16 : 16;
    uint32_t hw : 2;
} wideMoveOperand_t;
#pragma pack(pop)

typedef union {
    uint32_t raw;
    arithmeticOperand_t arithmeticOperand;
    wideMoveOperand_t wideMoveOperand;
} DPImmOperand_u;

#pragma pack(push, 1)
typedef struct {
    uint8_t : 1;
    uint8_t shift : 2;
    uint8_t : 1;
} arithmeticOpr_t;

#pragma pack(push, 1)
typedef struct {
    uint32_t : 6;
    uint32_t xm : 5;
    uint32_t : 1;
} SDTRegisterOffset_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint8_t N : 1;
    uint8_t shift : 2;
    uint8_t : 1;
} logicalOpr_t;
#pragma pack(pop)

typedef union {
    uint8_t raw;
    arithmeticOpr_t arithmetic;
    logicalOpr_t logical;
} DPRegOpr_u;

#pragma pack(push, 1)
typedef struct {
    uint32_t : 1;
    uint32_t i : 1;
    int32_t simm9 : 9;
    uint32_t : 1;
} SDTPrePostIndex_t;
#pragma pack(pop)

typedef union {
    uint32_t raw;
    SDTRegisterOffset_t registerOffset;
    SDTPrePostIndex_t prePostIndex;
} SDTOffset_u;

extern void executeDPImm(processorState_t *state, DPImmInstruction_t dpimm);

bool decodeAndExecute(processorState_t *state, uint32_t rawInstruction);
