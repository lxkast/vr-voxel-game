#pragma once

#include <stdint.h>
#include "decoder.h"

#define OPI_ARITHMETIC 2
#define OPI_WIDE_MOVE 5

typedef void (*ArithmeticOperation)(processorState_t *state, DPImmInstruction_t instruction, arithmeticOperand_t operand);

typedef void (*WideMoveOperation)(processorState_t *state, DPImmInstruction_t instruction, wideMoveOperand_t operand);

extern void executeAdd(processorState_t *state, DPImmInstruction_t instruction, arithmeticOperand_t operand);
extern void executeAdds(processorState_t *state, DPImmInstruction_t instruction, arithmeticOperand_t operand);
extern void executeSub(processorState_t *state, DPImmInstruction_t instruction, arithmeticOperand_t operand);
extern void executeSubs(processorState_t *state, DPImmInstruction_t instruction, arithmeticOperand_t operand);

extern void executeMovn(processorState_t *state, DPImmInstruction_t instruction, wideMoveOperand_t operand);
extern void executeMovz(processorState_t *state, DPImmInstruction_t instruction, wideMoveOperand_t operand);
extern void executeMovk(processorState_t *state, DPImmInstruction_t instruction, wideMoveOperand_t operand);

extern void executeDPImm(processorState_t *state, DPImmInstruction_t instruction);

typedef void (*LogicalOperation)(processorState_t *state, DPRegInstruction_t instruction, logicalOpr_t opr);

extern void executeAnd(processorState_t *state, DPRegInstruction_t instruction, logicalOpr_t opr);
extern void executeBic(processorState_t *state, DPRegInstruction_t instruction, logicalOpr_t opr);
extern void executeOrr(processorState_t *state, DPRegInstruction_t instruction, logicalOpr_t opr);
extern void executeOrn(processorState_t *state, DPRegInstruction_t instruction, logicalOpr_t opr);
extern void executeEor(processorState_t *state, DPRegInstruction_t instruction, logicalOpr_t opr);
extern void executeEon(processorState_t *state, DPRegInstruction_t instruction, logicalOpr_t opr);
extern void executeAnds(processorState_t *state, DPRegInstruction_t instruction, logicalOpr_t opr);
extern void executeBics(processorState_t *state, DPRegInstruction_t instruction, logicalOpr_t opr);

extern uint64_t lsl64(uint64_t rm, uint64_t operand);
extern uint64_t lsr64(uint64_t rm, uint64_t operand);
extern uint64_t asr64(uint64_t rm, uint64_t operand);
extern uint64_t ror64(uint64_t rm, uint64_t operand);
extern uint32_t lsl32(uint32_t rm, uint32_t operand);
extern uint32_t lsr32(uint32_t rm, uint32_t operand);
extern uint32_t asr32(uint32_t rm, uint32_t operand);
extern uint32_t ror32(uint32_t rm, uint32_t operand);

typedef uint64_t (*BitWise64Operation)(uint64_t rm, uint64_t operand);
typedef uint32_t (*BitWise32Operation)(uint32_t rm, uint32_t operand);

extern void executeDPReg(processorState_t *state, DPRegInstruction_t instruction);

typedef void (*BranchOperation)(processorState_t *state, branchOperand_t operand);

extern void executeBranch(processorState_t *state, branchInstruction_t operation);

extern void executeLoadLiteral(processorState_t *state, loadLitInstruction_t operation);