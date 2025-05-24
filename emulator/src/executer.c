#include "executer.h"
#include "logging.h"

/*
    Defining the functions for arithmetic operations. NOTE: these do not correctly handle
    the optional cases when rn or rd = 11111.
*/

void executeAdd(processorState_t *state, const DPImmInstruction_t instruction) {
    const arithmeticOperand_t operand = *(const arithmeticOperand_t*)&instruction.operand;

    if (instruction.sf) {
        const uint64_t value = read_gpReg64(state, operand.rn) + (operand.imm12 << (operand.sh * 12));
        write_gpReg64(state, instruction.rd, value);
    } else {
        const uint32_t value = read_gpReg32(state, operand.rn) + (operand.imm12 << (operand.sh * 12));
        write_gpReg32(state, instruction.rd, value);
    }
};

void executeAdds(processorState_t *state, const DPImmInstruction_t instruction);

void executeSub(processorState_t *state, const DPImmInstruction_t instruction) {
    const arithmeticOperand_t operand = *(const arithmeticOperand_t*)&instruction.operand;

    if (instruction.sf) {
        const uint64_t value = read_gpReg64(state, operand.rn) - (operand.imm12 << (operand.sh * 12));
        write_gpReg64(state, instruction.rd, value);
    } else {
        const uint32_t value = read_gpReg32(state, operand.rn) - (operand.imm12 << (operand.sh * 12));
        write_gpReg32(state, instruction.rd, value);
    }
};

void executeSubs(processorState_t *state, const DPImmInstruction_t instruction);

// Creating an array of function pointers
ArithmeticOperation arithmeticOperations[] = {
    executeAdd,
    executeAdds,
    executeSub,
    executeSubs
};

// executes a DP immediate instruction
void executeDPImm(processorState_t *state, const DPImmInstruction_t instruction) {
    if (instruction.opi == OPI_ARITHMETIC) {
        // Execute arithmetic instruction
        if (instruction.opc < 4) {
            arithmeticOperations[instruction.opc](state, instruction);
        } else {
            LOG_FATAL("Unsupported arithmetic operation");
        }
    } else if (instruction.opi == OPI_WIDE_MOVE) {
        // TODO: Execute wide move instruction
    } else {
        LOG_FATAL("Unsupported instruction type");
    }
}