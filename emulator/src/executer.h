#pragma once

#include "decoder.h"

extern void executeDPImm(processorState_t *state, DPImmInstruction_t instruction);

extern void executeSDT(processorState_t *state, SDTInstruction_t instruction);

extern void executeDPReg(processorState_t *state, DPRegInstruction_t instruction);

extern void executeBranch(processorState_t *state, branchInstruction_t operation);

extern void executeLoadLiteral(processorState_t *state, loadLitInstruction_t operation);