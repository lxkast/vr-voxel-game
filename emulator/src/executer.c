#include "executer.h"
#include "logging.h"

/*
    Defining the functions for arithmetic operations. NOTE: these do not correctly handle
    the optional cases when rn or rd = 11111.
*/

void executeAdd(processorState_t *state, const DPImmInstruction_t instruction) {
    const arithmeticOperand_t operand = *(const arithmeticOperand_t*)&instruction.operand;
    const uint64_t op2 = operand.imm12 << (operand.sh * 12);

    if (instruction.sf) {
        const uint64_t result = read_gpReg64(state, operand.rn) + op2;
        write_gpReg64(state, instruction.rd, result);
    } else {
        const uint32_t result = read_gpReg32(state, operand.rn) + op2;
        write_gpReg32(state, instruction.rd, result);
    }
};

void executeAdds(processorState_t *state, const DPImmInstruction_t instruction) {
    const arithmeticOperand_t operand = *(const arithmeticOperand_t*)&instruction.operand;
    const uint64_t op2 = operand.imm12 << (operand.sh * 12);

    if (instruction.sf) {
        const uint64_t rn = read_gpReg64(state, operand.rn);
        const uint64_t result = rn + op2;

        // Setting flags
        state->spRegisters.PSTATE.N = result >> 63;     // Negative flag
        state->spRegisters.PSTATE.Z = result == 0;      // Zero flag
        state->spRegisters.PSTATE.C = result < op2;     // Carry flag
        state->spRegisters.PSTATE.V = ((rn ^ op2) & (rn ^ result)) < 0;   // signed overflow/underflow flag

        write_gpReg64(state, instruction.rd, result);
    } else {
        const uint64_t rn = read_gpReg32(state, operand.rn);
        const uint32_t result = read_gpReg32(state, operand.rn) + (uint32_t)op2;

        // Setting flags
        state->spRegisters.PSTATE.N = result >> 31;     // Negative flag
        state->spRegisters.PSTATE.Z = result == 0;      // Zero flag
        state->spRegisters.PSTATE.C = result < op2;     // Carry flag
        state->spRegisters.PSTATE.V = ((rn ^ op2) & (rn ^ result)) < 0;   // signed overflow/underflow flag

        write_gpReg32(state, instruction.rd, result);
    }
};

void executeSub(processorState_t *state, const DPImmInstruction_t instruction) {
    const arithmeticOperand_t operand = *(const arithmeticOperand_t*)&instruction.operand;
    const uint64_t op2 = operand.imm12 << (operand.sh * 12);

    if (instruction.sf) {
        const uint64_t result = read_gpReg64(state, operand.rn) - op2;
        write_gpReg64(state, instruction.rd, result);
    } else {
        const uint32_t result = read_gpReg32(state, operand.rn) - op2;
        write_gpReg32(state, instruction.rd, result);
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