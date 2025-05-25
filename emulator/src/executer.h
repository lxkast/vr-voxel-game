#pragma once

#include <stdint.h>
#include "decoder.h"

#define OPI_ARITHMETIC 2
#define OPI_WIDE_MOVE 5

typedef struct {
    uint8_t sh : 1;
    uint16_t imm12 : 12;
    uint8_t rn : 5;
} arithmeticOperand_t;

typedef struct {
    uint8_t hw : 2;
    uint16_t imm16 : 16;
} wideMoveOperand_t;

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
void executeMovk(processorState_t *state, DPImmInstruction_t instruction, wideMoveOperand_t operand);
void executeMovz(processorState_t *state, DPImmInstruction_t instruction, wideMoveOperand_t operand);

extern void executeDPImm(processorState_t *state, DPImmInstruction_t instruction);