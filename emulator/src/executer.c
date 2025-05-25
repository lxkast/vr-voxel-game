#include <assert.h>
#include <logging.h>
#include "executer.h"

#include "memory.h"

/*
    Defining the functions for arithmetic operations. NOTE: these do not correctly handle
    the optional cases when rn or rd = 11111.
*/

_Static_assert(sizeof(DPImmInstruction_t) == 4, "DPImmInstruction_t must be 4 bytes");
_Static_assert(sizeof(DPRegInstruction_t) == 4, "DPRegInstruction_t must be 4 bytes");
_Static_assert(sizeof(SDTInstruction_t) == 4, "SDTInstruction_t must be 4 bytes");
_Static_assert(sizeof(loadLitInstruction_t) == 4, "loadLitInstruction_t must be 4 bytes");
_Static_assert(sizeof(branchInstruction_t) == 4, "branchInstruction_t must be 4 bytes");
_Static_assert(sizeof(instruction_u) == 4, "instruction_u must be 4 bytes");

void executeAdd(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    const uint64_t op2 = ((uint64_t) operand.imm12) << (operand.sh * 12);

    if (instruction.sf) {
        const uint64_t result = read_gpReg64(state, operand.rn) + op2;
        write_gpReg64(state, instruction.rd, result);
    } else {
        // op2 is guaranteed to be <= 32 bits, so casting it down to uint32_t is fine
        const uint32_t result = read_gpReg32(state, operand.rn) + (uint32_t)op2;
        write_gpReg32(state, instruction.rd, result);
    }
}

// performs the add operation, storing values in flags
void executeAdds(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    if (instruction.sf) {
        const uint64_t op2 = ((uint64_t) operand.imm12) << (operand.sh * 12);
        const uint64_t rn = read_gpReg64(state, operand.rn);
        const uint64_t result = rn + op2;

        // Setting flags

        pState_t pState;
        pState.N = result >> 63;     // Negative flag
        pState.Z = result == 0;      // Zero flag
        pState.C = result < rn;     // Carry flag
        pState.V = ((rn ^ op2) & ~(result ^ op2)) >> 63;   // signed overflow/underflow flag

        write_pState(state, pState);
        write_gpReg64(state, instruction.rd, result);
    } else {
        const uint32_t op2 = operand.imm12 << (operand.sh * 12);
        const uint32_t rn = read_gpReg32(state, operand.rn);
        const uint32_t result = rn + op2;

        // Setting flags
        pState_t pState;
        pState.N = result >> 31;     // Negative flag
        pState.Z = result == 0;      // Zero flag
        pState.C = result < rn;     // Carry flag
        pState.V = ((rn ^ op2) & ~(result ^ op2)) >> 31;   // signed overflow/underflow flag

        write_pState(state, pState);
        write_gpReg32(state, instruction.rd, result);
    }
}

void executeSub(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    const uint64_t op2 = ((uint64_t) operand.imm12) << (operand.sh * 12);

    if (instruction.sf) {
        const uint64_t result = read_gpReg64(state, operand.rn) - op2;
        write_gpReg64(state, instruction.rd, result);
    } else {
        // op2 is guaranteed to be <= 32 bits, so casting it down to uint32_t is fine
        const uint32_t result = read_gpReg32(state, operand.rn) - (uint32_t)op2;
        write_gpReg32(state, instruction.rd, result);
    }
}

// performs the sub operation, storing values in flags
void executeSubs(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    if (instruction.sf) {
        const uint64_t op2 = ((uint64_t) operand.imm12) << (operand.sh * 12);
        const uint64_t rn = read_gpReg64(state, operand.rn);
        const uint64_t result = rn - op2;

        pState_t pState;
        pState.N = result >> 63;     // Negative flag
        pState.Z = result == 0;      // Zero flag
        pState.C = rn >= op2;     // Carry flag
        pState.V = ((rn ^ op2) & ~(result ^ op2)) >> 63;   // signed overflow/underflow flag

        write_pState(state, pState);
        write_gpReg64(state, instruction.rd, result);
    } else {
        const uint32_t op2 = operand.imm12 << (operand.sh * 12);
        const uint32_t rn = read_gpReg32(state, operand.rn);
        const uint32_t result = rn - op2;

        // Setting flags
        pState_t pState;
        pState.N = result >> 31;     // Negative flag
        pState.Z = result == 0;      // Zero flag
        pState.C = rn >= op2;        // Carry flag
        pState.V = ((rn ^ op2) & ~(result ^ op2)) >> 31;   // signed overflow/underflow flag

        write_pState(state, pState);
        write_gpReg32(state, instruction.rd, result);
    }
}

/*
    Defining the functions for wide move operations.
*/

void executeMovn(processorState_t *state, const DPImmInstruction_t instruction, const wideMoveOperand_t operand) {
    const uint64_t op = ((uint64_t) operand.imm16) << (operand.hw * 16);
    if (instruction.sf) {
        const uint64_t result = ~op;
        write_gpReg64(state, instruction.rd, result);
    } else {
        if (operand.hw > 1) {
            LOG_FATAL("executeMovn: operand.hw > 1 (in 32bit mode)");
        }
        // op is guaranteed to be <= 32 bits, so casting it down to uint32_t is fine
        const uint32_t result = ~(uint32_t)op;
        write_gpReg32(state, instruction.rd, result);
    }
}

void executeMovz(processorState_t *state, const DPImmInstruction_t instruction, const wideMoveOperand_t operand) {
    const uint64_t op = ((uint64_t) operand.imm16) << (operand.hw * 16);
    if (instruction.sf) {
        const uint64_t result = op;
        write_gpReg64(state, instruction.rd, result);
    } else {
        if (operand.hw > 1) {
            LOG_FATAL("executeMovz: operand.hw > 1 (in 32bit mode)");
        }
        // op is guaranteed to be <= 32 bits, so casting it down to uint32_t is fine
        const uint32_t result = (uint32_t)op;
        write_gpReg32(state, instruction.rd, result);
    }
}

void executeMovk(processorState_t *state, const DPImmInstruction_t instruction, const wideMoveOperand_t operand) {
    const uint64_t op = ((uint64_t) operand.imm16) << (operand.hw * 16);
    const uint64_t mask = ~(0xFFFFl << (operand.hw * 16));
    if (instruction.sf) {
        const uint64_t rd = read_gpReg64(state,instruction.rd);
        const uint64_t result = rd & mask | op;
        write_gpReg64(state, instruction.rd, result);
    } else {
        if (operand.hw > 1) {
            LOG_FATAL("executeMovk: operand.hw > 1 (in 32bit mode)");
        }
        // op and mask are guaranteed to be <= 32 bits, so casting them down to uint32_t is fine
        const uint32_t rd = read_gpReg32(state,instruction.rd);
        const uint32_t result = rd & (uint32_t)mask | (uint32_t)op;
        write_gpReg32(state, instruction.rd, result);
    }
}
// this is used to allow function pointers to work more neatly
void executeNoOp() {
    LOG_FATAL("No op executed");
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
        LOG_FATAL("Unsupported instruction type for DPImm instruction");
    }
}

uint64_t lsl64(const uint64_t rm, const uint64_t operand) {
    return rm << operand;
}
uint64_t lsr64(const uint64_t rm, const uint64_t operand) {
    return rm >> operand;
}
uint64_t asr64(const uint64_t rm, const uint64_t operand) {
    return rm >> operand | (rm & 0x8000000000000000);
}
uint64_t ror64(const uint64_t rm, const uint64_t operand) {
    return rm >> operand | rm << (64 - operand);
}

uint32_t lsl32(const uint32_t rm, const uint32_t operand) {
    return rm << operand;
}
uint32_t lsr32(const uint32_t rm, const uint32_t operand) {
    return rm >> operand;
}
uint32_t asr32(const uint32_t rm, const uint32_t operand) {
    return rm >> operand | (rm & 0x80000000);
}
uint32_t ror32(const uint32_t rm, const uint32_t operand) {
    return rm >> operand | rm << (32 - operand);
}

BitWise32Operation bitWise32Operations[] = {
    lsl32,
    lsr32,
    asr32,
    ror32,
};

BitWise64Operation bitWise64Operations[] = {
    lsl64,
    lsr64,
    asr64,
    ror64,
};

void executeAnd(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitWise64Operations[opr.shift](read_gpReg64(state,instruction.rm),instruction.operand);
        const uint64_t result = read_gpReg64(state,instruction.rn) & op2;
        write_gpReg64(state, instruction.rm, result);
    } else {
        const uint32_t op2 = bitWise32Operations[opr.shift](read_gpReg32(state,instruction.rm),instruction.operand);
        const uint32_t result = read_gpReg32(state,instruction.rn) & op2;
        write_gpReg32(state, instruction.rm, result);
    }
}
void executeBic(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitWise64Operations[opr.shift](read_gpReg64(state,instruction.rm),instruction.operand);
        const uint64_t result = read_gpReg64(state,instruction.rn) & ~op2;
        write_gpReg64(state, instruction.rm, result);
    } else {
        const uint32_t op2 = bitWise32Operations[opr.shift](read_gpReg32(state,instruction.rm),instruction.operand);
        const uint32_t result = read_gpReg32(state,instruction.rn) & ~op2;
        write_gpReg32(state, instruction.rm, result);
    }
}
void executeOrr(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitWise64Operations[opr.shift](read_gpReg64(state,instruction.rm),instruction.operand);
        const uint64_t result = read_gpReg64(state,instruction.rn) | op2;
        write_gpReg64(state, instruction.rm, result);
    } else {
        const uint32_t op2 = bitWise32Operations[opr.shift](read_gpReg32(state,instruction.rm),instruction.operand);
        const uint32_t result = read_gpReg32(state,instruction.rn) | op2;
        write_gpReg32(state, instruction.rm, result);
    }
}
void executeOrn(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitWise64Operations[opr.shift](read_gpReg64(state,instruction.rm),instruction.operand);
        const uint64_t result = read_gpReg64(state,instruction.rn) | ~op2;
        write_gpReg64(state, instruction.rm, result);
    } else {
        const uint32_t op2 = bitWise32Operations[opr.shift](read_gpReg32(state,instruction.rm),instruction.operand);
        const uint32_t result = read_gpReg32(state,instruction.rn) | ~op2;
        write_gpReg32(state, instruction.rm, result);
    }
}
void executeEor(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitWise64Operations[opr.shift](read_gpReg64(state,instruction.rm),instruction.operand);
        const uint64_t result = read_gpReg64(state,instruction.rn) ^ op2;
        write_gpReg64(state, instruction.rm, result);
    } else {
        const uint32_t op2 = bitWise32Operations[opr.shift](read_gpReg32(state,instruction.rm),instruction.operand);
        const uint32_t result = read_gpReg32(state,instruction.rn) ^ op2;
        write_gpReg32(state, instruction.rm, result);
    }
}
void executeEon(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitWise64Operations[opr.shift](read_gpReg64(state,instruction.rm),instruction.operand);
        const uint64_t result = read_gpReg64(state,instruction.rn) ^ ~op2;
        write_gpReg64(state, instruction.rm, result);
    } else {
        const uint32_t op2 = bitWise32Operations[opr.shift](read_gpReg32(state,instruction.rm),instruction.operand);
        const uint32_t result = read_gpReg32(state,instruction.rn) ^ ~op2;
        write_gpReg32(state, instruction.rm, result);
    }
}
void executeAnds(processorState_t *state, DPRegInstruction_t instruction, logicalOpr_t opr) {}
void executeBics(processorState_t *state, DPRegInstruction_t instruction, logicalOpr_t opr) {}

LogicalOperation logicalOperations[] = {
    executeAnd,
    executeBic,
    executeOrr,
    executeOrn,
    executeEor,
    executeEon,
    executeAnds,
    executeBics,
};

void executeDPReg(processorState_t *state, const DPRegInstruction_t instruction) {
    // I had ~instruction.m, but the compiler said that would always be 1
    if (instruction.m == 0 && (instruction.opr | 0x6) == 0xE) {
        const DPRegOpr_u DPopr = {.raw = instruction.opr};
        const logicalOpr_t opr = DPopr.logical;
        logicalOperations[(instruction.opc << 1) + opr.N](state, instruction, opr);
    } else if (instruction.m == 0 && (instruction.opr | 0x7) == 0x7) {
        // TODO: Execute Logical Instruction
    } else if (instruction.m && instruction.opr == 0x8) {
        // TODO: Execute multiply instruction
    } else {
        LOG_FATAL("Unsupported instruction type for DPReg instruction");
    }
}

void registerBranch(processorState_t *state, const branchOperand_t operand) {
    if ((operand.branchRegister.xn & 0x1F) == 0x1F) {
        LOG_FATAL("Branch to xzr not supported");
    }
    const uint64_t registerValue = read_gpReg64(state, operand.branchRegister.xn);
    write_PC(state, registerValue);
}

void unconditionalBranch(processorState_t *state, const branchOperand_t operand) {
    LOG_DEBUG("uncond branch");
    increment_PC(state, operand.branchUnconditional.offset * 4);
}

bool evalCondition(processorState_t *state, const uint32_t condition) {
    const pState_t flags = state->spRegisters.PSTATE;
    switch (condition) {
        case 0x0: return flags.Z;
        case 0x1: return !flags.Z;
        case 0xA: return flags.N == flags.V;
        case 0xB: return flags.N != flags.V;
        case 0xC: return !flags.Z & (flags.N == flags.V);
        case 0xD: return !(!flags.Z & (flags.N == flags.V));
        case 0xE: return true;
        default: LOG_FATAL("Unsupported condition code (%x)", condition);
    }
}

void conditionalBranch(processorState_t *state, branchOperand_t operand) {
    if (evalCondition(state, operand.branchCondition.cond)) {
        increment_PC(state, operand.branchCondition.offset * 4);
    } else {
        increment_PC(state, 4);
    }
}

BranchOperation branchOperations[] = {unconditionalBranch, conditionalBranch, executeNoOp, registerBranch};

void executeBranch(processorState_t *state, const branchInstruction_t instruction) {
    const branchOperand_t operand = { .raw=instruction.operand };
    branchOperations[instruction.type](state, operand);
}

void executeLoadLiteral(processorState_t *state, loadLitInstruction_t operation) {
    if (operation.rt == 0x1F) LOG_FATAL("SP not supported");
    uint32_t address = read_PC(state) + operation.simm19 * 4;
    if (operation.sf) {
        uint64_t data = read_mem64(state, address);
        write_gpReg64(state, operation.rt, data);
    } else {
        uint32_t data = read_mem32(state, address);
        write_gpReg32(state, operation.rt, data);
    }
}