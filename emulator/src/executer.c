#include "executer.h"
#include "logging.h"

/*
    Defining the functions for arithmetic operations. NOTE: these do not correctly handle
    the optional cases when rn or rd = 11111.
*/

void executeAdd(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    const uint64_t op2 = operand.imm12 << (operand.sh * 12);

    if (instruction.sf) {
        const uint64_t result = read_gpReg64(state, operand.rn) + op2;
        write_gpReg64(state, instruction.rd, result);
    } else {
        const uint32_t result = read_gpReg32(state, operand.rn) + (uint32_t)op2;
        write_gpReg32(state, instruction.rd, result);
    }
}

void executeAdds(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    if (instruction.sf) {
        const uint64_t op2 = operand.imm12 << (operand.sh * 12);
        const uint64_t rn = read_gpReg64(state, operand.rn);
        const uint64_t result = rn + op2;

        // Setting flags
        state->spRegisters.PSTATE.N = result >> 63;     // Negative flag
        state->spRegisters.PSTATE.Z = result == 0;      // Zero flag
        state->spRegisters.PSTATE.C = result < rn;     // Carry flag
        state->spRegisters.PSTATE.V = ((rn ^ op2) & ~(op2 ^ result)) >> 63;   // signed overflow/underflow flag

        write_gpReg64(state, instruction.rd, result);
    } else {
        const uint32_t op2 = operand.imm12 << (operand.sh * 12);
        const uint32_t rn = read_gpReg32(state, operand.rn);
        const uint32_t result = rn + op2;

        // Setting flags
        state->spRegisters.PSTATE.N = result >> 31;     // Negative flag
        state->spRegisters.PSTATE.Z = result == 0;      // Zero flag
        state->spRegisters.PSTATE.C = result < rn;     // Carry flag
        state->spRegisters.PSTATE.V = ((rn ^ op2) & ~(op2 ^ result)) >> 31;   // signed overflow/underflow flag

        write_gpReg32(state, instruction.rd, result);
    }
}

void executeSub(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    const uint64_t op2 = operand.imm12 << (operand.sh * 12);

    if (instruction.sf) {
        const uint64_t result = read_gpReg64(state, operand.rn) - op2;
        write_gpReg64(state, instruction.rd, result);
    } else {
        const uint32_t result = read_gpReg32(state, operand.rn) - (uint32_t)op2;
        write_gpReg32(state, instruction.rd, result);
    }
}

void executeSubs(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    if (instruction.sf) {
        const uint64_t op2 = operand.imm12 << (operand.sh * 12);
        const uint64_t rn = read_gpReg64(state, operand.rn);
        const uint64_t result = rn - op2;

        // Setting flags
        state->spRegisters.PSTATE.N = result >> 63;     // Negative flag
        state->spRegisters.PSTATE.Z = result == 0;      // Zero flag
        state->spRegisters.PSTATE.C = rn >= op2;     // Carry flag
        state->spRegisters.PSTATE.V = ((rn ^ result) & ~(rn ^ op2)) >> 63;   // signed overflow/underflow flag

        write_gpReg64(state, instruction.rd, result);
    } else {
        const uint32_t op2 = operand.imm12 << (operand.sh * 12);
        const uint32_t rn = read_gpReg32(state, operand.rn);
        const uint32_t result = rn - op2;

        // Setting flags
        state->spRegisters.PSTATE.N = result >> 31;     // Negative flag
        state->spRegisters.PSTATE.Z = result == 0;      // Zero flag
        state->spRegisters.PSTATE.C = rn >= op2;     // Carry flag
        state->spRegisters.PSTATE.V = ((rn ^ result) & ~(rn ^ op2)) >> 31;   // signed overflow/underflow flag

        write_gpReg32(state, instruction.rd, result);
    }
}

void executeMovn(processorState_t *state, const DPImmInstruction_t instruction, const wideMoveOperand_t operand) {
    const uint64_t op = operand.imm16 << (operand.hw * 16);
    if (instruction.sf) {
        const uint64_t result = ~op;
        write_gpReg64(state, instruction.rd, result);
    } else {
        if (operand.hw > 1) {
            LOG_FATAL("executeMovn: operand.hw > 1 (in 32bit mode)");
        }
        const uint32_t result = ~(uint32_t)op;
        write_gpReg32(state, instruction.rd, result);
    }
}

void executeMovk(processorState_t *state, const DPImmInstruction_t instruction, const wideMoveOperand_t operand) {
    const uint64_t op = operand.imm16 << (operand.hw * 16);
    if (instruction.sf) {
        const uint64_t result = op;
        write_gpReg64(state, instruction.rd, result);
    } else {
        if (operand.hw > 1) {
            LOG_FATAL("executeMovn: operand.hw > 1 (in 32bit mode)");
        }
        const uint32_t result = (uint32_t)op;
        write_gpReg32(state, instruction.rd, result);
    }
}

void executeMovz(processorState_t *state, DPImmInstruction_t instruction, wideMoveOperand_t operand) {}

void executeNoOp() {
    // intentionally empty
}

// Creating arrays of function pointers
ArithmeticOperation arithmeticOperations[] = {
    executeAdd,
    executeAdds,
    executeSub,
    executeSubs
};

WideMoveOperation wideMoveOperations[] = {
    executeMovn,
    executeNoOp,
    executeMovz,
    executeMovk
};

// executes a DP immediate instruction
void executeDPImm(processorState_t *state, const DPImmInstruction_t instruction) {
    if (instruction.opi == OPI_ARITHMETIC) {
        // Execute arithmetic instruction
        const DPImmOperand_u op = { .raw = instruction.operand };
        const arithmeticOperand_t operand = op.arithmeticOperand;
        arithmeticOperations[instruction.opc](state, instruction, operand);
    }else if (instruction.opi == OPI_WIDE_MOVE) {
        const DPImmOperand_u op = { .raw = instruction.operand };
        const wideMoveOperand_t operand = op.wideMoveOperand;
        wideMoveOperations[instruction.opc](state, instruction, operand);
    } else {
        LOG_FATAL("Unsupported instruction type");
    }
}