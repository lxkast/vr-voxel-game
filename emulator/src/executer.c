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

void writeReg64Z(processorState_t *state, register_t reg, uint64_t value) {
    if (reg == 0x1F) {
        write_ZR(state, value);
    } else {
        write_gpReg64(state, reg,  value);
    }
}
void writeReg32Z(processorState_t *state, register_t reg, uint32_t value) {
    if (reg == 0x1F) {
        write_ZR(state, value);
    } else {
        write_gpReg32(state, reg,  value);
    }
}
uint64_t readReg64Z(processorState_t *state, register_t reg) {
    if (reg == 0x1F) {
        return read_ZR(state);
    } else {
        return read_gpReg64(state, reg);
    }
}
uint32_t readReg32Z(processorState_t *state, register_t reg) {
    if (reg == 0x1F) {
        return read_ZR(state);
    } else {
        return read_gpReg32(state, reg);
    }
}


void add64(processorState_t *state, register_t dest, uint64_t op1, uint64_t op2) {
    const uint64_t result = op1 + op2;
    writeReg64Z(state, dest, result);
}

void add32(processorState_t *state, register_t dest, uint32_t op1, uint32_t op2) {
    const uint32_t result = op1 + op2;
    writeReg32Z(state, dest, result);
}

void adds64(processorState_t *state, register_t dest, uint64_t op1, uint64_t op2) {
    const uint64_t result = op1 + op2;

    // Setting flags

    pState_t pState;
    pState.N = result >> 63;     // Negative flag
    pState.Z = result == 0;      // Zero flag
    pState.C = result < op1;     // Carry flag
    pState.V = ((op1 ^ op2) & ~(result ^ op2)) >> 63;   // signed overflow/underflow flag

    write_pState(state, pState);
    writeReg64Z(state, dest, result);
}

void adds32(processorState_t *state, register_t dest, uint32_t op1, uint32_t op2) {
    const uint32_t result = op1 + op2;

    // Setting flags
    pState_t pState;
    pState.N = result >> 31;     // Negative flag
    pState.Z = result == 0;      // Zero flag
    pState.C = result < op1;     // Carry flag
    pState.V = ((op1 ^ op2) & ~(result ^ op2)) >> 31;   // signed overflow/underflow flag

    write_pState(state, pState);
    writeReg32Z(state, dest, result);
}

void sub64(processorState_t *state, register_t dest, uint64_t op1, uint64_t op2) {
    const uint64_t result = op1 - op2;
    writeReg64Z(state, dest, result);
}

void sub32(processorState_t *state, register_t dest, uint32_t op1, uint32_t op2) {
    const uint32_t result = op1 - op2;
    writeReg32Z(state, dest, result);
}

void subs64(processorState_t *state, register_t dest, uint64_t op1, uint64_t op2) {
    const uint64_t result = op1 - op2;

    pState_t pState;
    pState.N = result >> 63;     // Negative flag
    pState.Z = result == 0;      // Zero flag
    pState.C = op1 >= op2;     // Carry flag
    pState.V = ((op1 ^ op2) & ~(result ^ op2)) >> 63;   // signed overflow/underflow flag

    write_pState(state, pState);
    writeReg64Z(state, dest, result);
}

void subs32(processorState_t *state, register_t dest, uint32_t op1, uint32_t op2) {
    const uint32_t result = op1 - op2;

    // Setting flags
    pState_t pState;
    pState.N = result >> 31;     // Negative flag
    pState.Z = result == 0;      // Zero flag
    pState.C = op1 >= op2;        // Carry flag
    pState.V = ((op1 ^ op2) & ~(result ^ op2)) >> 31;   // signed overflow/underflow flag

    write_pState(state, pState);
    writeReg32Z(state, dest, result);
}

void executeAdd(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    const uint64_t op2 = ((uint64_t) operand.imm12) << (operand.sh * 12);
    if (instruction.sf) {
       add64(state, instruction.rd, readReg64Z(state, operand.rn), op2);
    } else {
        // op2 is guaranteed to be <= 32 bits, so casting it down to uint32_t is fine
        add32(state, instruction.rd, readReg32Z(state, operand.rn), op2);
    }
}

// performs the add operation, storing values in flags
void executeAdds(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    const uint64_t op2 = ((uint64_t) operand.imm12) << (operand.sh * 12);
    if (instruction.sf) {
        const uint64_t rn = readReg64Z(state, operand.rn);
        adds64(state, instruction.rd, rn, op2);
    } else {
        const uint32_t rn = readReg32Z(state, operand.rn);
        adds32(state, instruction.rd, rn, op2);
    }
}

void executeSub(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    const uint64_t op2 = ((uint64_t) operand.imm12) << (operand.sh * 12);
    if (instruction.sf) {
        sub64(state, instruction.rd, readReg64Z(state, operand.rn), op2);
    } else {
        // op2 is guaranteed to be <= 32 bits, so casting it down to uint32_t is fine
        sub32(state, instruction.rd, readReg32Z(state, operand.rn), op2);
    }
}

// performs the sub operation, storing values in flags
void executeSubs(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    const uint64_t op2 = ((uint64_t) operand.imm12) << (operand.sh * 12);
    if (instruction.sf) {
        const uint64_t rn = readReg64Z(state, operand.rn);
        subs64(state, instruction.rd, rn, op2);
    } else {
        const uint32_t rn = readReg32Z(state, operand.rn);
        subs32(state, instruction.rd, rn, op2);
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
        const uint64_t result = (rd & mask) | op;
        write_gpReg64(state, instruction.rd, result);
    } else {
        if (operand.hw > 1) {
            LOG_FATAL("executeMovk: operand.hw > 1 (in 32bit mode)");
        }
        // op and mask are guaranteed to be <= 32 bits, so casting them down to uint32_t is fine
        const uint32_t rd = read_gpReg32(state,instruction.rd);
        const uint32_t result = (rd & (uint32_t)mask) | (uint32_t)op;
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
    return (uint64_t)((int64_t)rm >> operand);
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
    return (uint32_t)((int32_t)rm >> operand);
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
        const uint64_t op2 = bitWise64Operations[opr.shift](readReg64Z(state,instruction.rm),instruction.operand);
        const uint64_t result = readReg64Z(state,instruction.rn) & op2;
        writeReg64Z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitWise32Operations[opr.shift](readReg32Z(state,instruction.rm),instruction.operand);
        const uint32_t result = readReg32Z(state,instruction.rn) & op2;
        writeReg32Z(state, instruction.rd, result);
    }
}
void executeBic(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitWise64Operations[opr.shift](readReg64Z(state,instruction.rm),instruction.operand);
        const uint64_t result = readReg64Z(state,instruction.rn) & ~op2;
        writeReg64Z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitWise32Operations[opr.shift](readReg32Z(state,instruction.rm),instruction.operand);
        const uint32_t result = readReg32Z(state,instruction.rn) & ~op2;
        writeReg32Z(state, instruction.rd, result);
    }
}
void executeOrr(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitWise64Operations[opr.shift](readReg64Z(state,instruction.rm),instruction.operand);
        const uint64_t result = readReg64Z(state,instruction.rn) | op2;
        writeReg64Z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitWise32Operations[opr.shift](readReg32Z(state,instruction.rm),instruction.operand);
        const uint32_t result = readReg32Z(state,instruction.rn) | op2;
        writeReg32Z(state, instruction.rd, result);
    }
}
void executeOrn(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitWise64Operations[opr.shift](readReg64Z(state,instruction.rm),instruction.operand);
        const uint64_t result = readReg64Z(state,instruction.rn) | ~op2;
        writeReg64Z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitWise32Operations[opr.shift](readReg32Z(state,instruction.rm),instruction.operand);
        const uint32_t result = readReg32Z(state,instruction.rn) | ~op2;
        writeReg32Z(state, instruction.rd, result);
    }
}
void executeEor(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitWise64Operations[opr.shift](readReg64Z(state,instruction.rm),instruction.operand);
        const uint64_t result = readReg64Z(state,instruction.rn) ^ op2;
        writeReg64Z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitWise32Operations[opr.shift](readReg32Z(state,instruction.rm),instruction.operand);
        const uint32_t result = readReg32Z(state,instruction.rn) ^ op2;
        writeReg32Z(state, instruction.rd, result);
    }
}
void executeEon(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitWise64Operations[opr.shift](readReg64Z(state,instruction.rm),instruction.operand);
        const uint64_t result = readReg64Z(state,instruction.rn) ^ ~op2;
        writeReg64Z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitWise32Operations[opr.shift](readReg32Z(state,instruction.rm),instruction.operand);
        const uint32_t result = readReg32Z(state,instruction.rn) ^ ~op2;
        writeReg32Z(state, instruction.rd, result);
    }
}
void executeAnds(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitWise64Operations[opr.shift](readReg64Z(state,instruction.rm),instruction.operand);
        const uint64_t result = readReg64Z(state,instruction.rn) & op2;

        pState_t pState;
        pState.N = result >> 63;
        pState.Z = result == 0;
        pState.C = 0;
        pState.V = 0;

        write_pState(state, pState);

        writeReg64Z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitWise32Operations[opr.shift](readReg32Z(state,instruction.rm),instruction.operand);
        const uint32_t result = readReg32Z(state,instruction.rn) & op2;

        pState_t pState;
        pState.N = result >> 31;
        pState.Z = result == 0;
        pState.C = 0;
        pState.V = 0;

        write_pState(state, pState);

        writeReg32Z(state, instruction.rd, result);
    }
}
void executeBics(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitWise64Operations[opr.shift](readReg64Z(state,instruction.rm),instruction.operand);
        const uint64_t result = readReg64Z(state,instruction.rn) & ~op2;

        pState_t pState;
        pState.N = result >> 63;
        pState.Z = result == 0;
        pState.C = 0;
        pState.V = 0;

        write_pState(state, pState);

        writeReg64Z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitWise32Operations[opr.shift](readReg32Z(state,instruction.rm),instruction.operand);
        const uint32_t result = readReg32Z(state,instruction.rn) & ~op2;

        pState_t pState;
        pState.N = result >> 31;
        pState.Z = result == 0;
        pState.C = 0;
        pState.V = 0;

        write_pState(state, pState);

        writeReg32Z(state, instruction.rd, result);
    }
}

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

void registerAdd(processorState_t *state, const DPRegInstruction_t instruction, const arithmeticOpr_t opr) {
    if (instruction.sf) {
        uint64_t rn = readReg64Z(state,instruction.rn);
        uint64_t rm = readReg64Z(state,instruction.rm);
        uint64_t op2 = bitWise64Operations[opr.shift](rm, instruction.operand);
        add64(state, instruction.rd,  rn, op2);
    } else {
        uint64_t rn = readReg32Z(state,instruction.rn);
        uint64_t rm = readReg32Z(state,instruction.rm);

        uint64_t op2 = bitWise32Operations[opr.shift](rm, instruction.operand);
        add32(state, instruction.rd,  rn, op2);
    }
}
void registerAddS(processorState_t *state, const DPRegInstruction_t instruction, const arithmeticOpr_t opr) {
    if (instruction.sf) {
        uint64_t rn = readReg64Z(state,instruction.rn);
        uint64_t rm = readReg64Z(state,instruction.rm);
        uint64_t op2 = bitWise64Operations[opr.shift](rm, instruction.operand);
        adds64(state, instruction.rd,  rn, op2);
    } else {
        uint64_t rn = readReg32Z(state,instruction.rn);
        uint64_t rm = readReg32Z(state,instruction.rm);
        uint64_t op2 = bitWise32Operations[opr.shift](rm, instruction.operand);

        adds32(state, instruction.rd,  rn, op2);
    }
}
void registerSub(processorState_t *state, const DPRegInstruction_t instruction, const arithmeticOpr_t opr) {
    if (instruction.sf) {
        uint64_t rn = readReg64Z(state,instruction.rn);
        uint64_t rm = readReg64Z(state,instruction.rm);
        uint64_t op2 = bitWise64Operations[opr.shift](rm, instruction.operand);
        sub64(state, instruction.rd,  rn, op2);
    } else {
        uint64_t rn = readReg32Z(state,instruction.rn);
        uint64_t rm = readReg32Z(state,instruction.rm);
        uint64_t op2 = bitWise32Operations[opr.shift](rm, instruction.operand);

        sub32(state, instruction.rd,  rn, op2);
    }
}
void registerSubs(processorState_t *state, const DPRegInstruction_t instruction, const arithmeticOpr_t opr) {
    if (instruction.sf) {
        uint64_t rn = readReg64Z(state,instruction.rn);
        uint64_t rm = readReg64Z(state,instruction.rm);
        uint64_t op2 = bitWise64Operations[opr.shift](rm, instruction.operand);
        subs64(state, instruction.rd,  rn, op2);
    } else {
        uint64_t rn = readReg32Z(state,instruction.rn);
        uint64_t rm = readReg32Z(state,instruction.rm);
        uint64_t op2 = bitWise32Operations[opr.shift](rm, instruction.operand);

        subs32(state, instruction.rd,  rn, op2);
    }
}

ArithmeticRegOperation arithmeticRegisterOperations[] = {registerAdd, registerAddS, registerSub, registerSubs};

void executeDPReg(processorState_t *state, const DPRegInstruction_t instruction) {
    // I had ~instruction.m, but the compiler said that would always be 1
    if (instruction.m == 0 && (instruction.opr | 0x6) == 0xE) {
        const DPRegOpr_u DPopr = {.raw = instruction.opr};
        const arithmeticOpr_t opr = DPopr.arithmetic;
        arithmeticRegisterOperations[instruction.opc](state, instruction, opr);
    } else if (instruction.m == 0 && (instruction.opr | 0x7) == 0x7) {
        const DPRegOpr_u DPopr = {.raw = instruction.opr};
        const logicalOpr_t opr = DPopr.logical;
        logicalOperations[(instruction.opc << 1) + opr.N](state, instruction, opr);
    } else if (instruction.m && instruction.opr == 0x8) {
        LOG_FATAL("MULTIPLY NOT IMPLEMENTED");
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