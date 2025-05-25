#pragma once

#include <stdint.h>
#include "decoder.h"

#define OPI_ARITHMETIC 2
#define OPI_WIDE_MOVE 5

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

typedef void (*ArithmeticOperation)(processorState_t *state, DPImmInstruction_t instruction, arithmeticOperand_t operand);

typedef void (*WideMoveOperation)(processorState_t *state, DPImmInstruction_t instruction, wideMoveOperand_t operand);

extern void executeAdd(processorState_t *state, DPImmInstruction_t instruction, arithmeticOperand_t operand);
extern void executeAdds(processorState_t *state, DPImmInstruction_t instruction, arithmeticOperand_t operand);
extern void executeSub(processorState_t *state, DPImmInstruction_t instruction, arithmeticOperand_t operand);
extern void executeSubs(processorState_t *state, DPImmInstruction_t instruction, arithmeticOperand_t operand);

void executeMovn(processorState_t *state, DPImmInstruction_t instruction, wideMoveOperand_t operand);
void executeMovz(processorState_t *state, DPImmInstruction_t instruction, wideMoveOperand_t operand);
void executeMovk(processorState_t *state, DPImmInstruction_t instruction, wideMoveOperand_t operand);

extern void executeDPImm(processorState_t *state, DPImmInstruction_t instruction);


typedef void (*BranchOperation)(processorState_t *state, branchOperand_t operand);

extern void executeBranch(processorState_t *state, branchInstruction_t operation);

extern void executeLoadLiteral(processorState_t *state, loadLitInstruction_t operation);