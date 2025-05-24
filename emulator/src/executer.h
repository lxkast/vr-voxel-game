#pragma once

#include <stdint.h>
#include "state.h"
#include "decoder.h"

#define OPI_ARITHMETIC 2
#define OPI_WIDE_MOVE 5

typedef void (*ArithmeticOperation)(processorState_t *state, const DPImmInstruction_t instruction);

typedef struct {
    uint8_t sh : 1;
    uint16_t imm12 : 12;
    uint8_t rn : 5;
} arithmeticOperand_t;

extern void executeDPImm(processorState_t *state, const DPImmInstruction_t instruction);