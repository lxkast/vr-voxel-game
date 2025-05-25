#include <assert.h>
#include <logging.h>
#include "executer.h"

#include "memory.h"

/*
    Defining the functions for arithmetic operations. NOTE: these do not correctly handle
    the optional cases when rn or rd = 11111.
*/
#define STACK_POINTER 0x1F

#define REGISTER_OFFSET_MASK 0x83F    // 2^11 + 2^5 + 2^4 + ... + 2^0
#define REGISTER_OFFSET_VALUE 0x81A   // 2^11 + 2^4 + 2^3 + 2^1

#define PRE_POST_INDEX_MASK 0x801     // 2^11 + 2^0
#define PRE_POST_INDEX_VALUE 0x1      // 2^0

_Static_assert(sizeof(DPImmInstruction_t) == 4, "DPImmInstruction_t must be 4 bytes");
_Static_assert(sizeof(DPRegInstruction_t) == 4, "DPRegInstruction_t must be 4 bytes");
_Static_assert(sizeof(SDTInstruction_t) == 4, "SDTInstruction_t must be 4 bytes");
_Static_assert(sizeof(loadLitInstruction_t) == 4, "loadLitInstruction_t must be 4 bytes");
_Static_assert(sizeof(branchInstruction_t) == 4, "branchInstruction_t must be 4 bytes");

void op_write_gpReg64(processorState_t *state, register_t reg, uint64_t value) {
    if (reg == 0x1F) {
        write_ZR(state, value);
    } else {
        write_gpReg64(state, reg,  value);
    }
}
void op_write_gpReg32(processorState_t *state, register_t reg, uint32_t value) {
    if (reg == 0x1F) {
        write_ZR(state, value);
    } else {
        write_gpReg32(state, reg,  value);
    }
}

void executeAdd(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    const uint64_t op2 = ((uint64_t) operand.imm12) << (operand.sh * 12);

    if (instruction.sf) {
        const uint64_t result = read_gpReg64(state, operand.rn) + op2;
        op_write_gpReg64(state, instruction.rd, result);
    } else {
        // op2 is guaranteed to be <= 32 bits, so casting it down to uint32_t is fine
        const uint32_t result = read_gpReg32(state, operand.rn) + (uint32_t)op2;
        op_write_gpReg32(state, instruction.rd, result);
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
        op_write_gpReg64(state, instruction.rd, result);
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
        op_write_gpReg32(state, instruction.rd, result);
    }
}

void executeSub(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    const uint64_t op2 = ((uint64_t) operand.imm12) << (operand.sh * 12);

    if (instruction.sf) {
        const uint64_t result = read_gpReg64(state, operand.rn) - op2;
        op_write_gpReg64(state, instruction.rd, result);
    } else {
        // op2 is guaranteed to be <= 32 bits, so casting it down to uint32_t is fine
        const uint32_t result = read_gpReg32(state, operand.rn) - (uint32_t)op2;
        op_write_gpReg32(state, instruction.rd, result);
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
        op_write_gpReg64(state, instruction.rd, result);
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
        op_write_gpReg32(state, instruction.rd, result);
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
        LOG_FATAL("Unsupported instruction type");
    }
}

void registerBranch(processorState_t *state, branchOperand_t operand) {
    if ((operand.branchRegister.xn & 0x1F) == 0x1F) {
        LOG_FATAL("Branch to xzr not supported");
    }
    uint64_t registerValue = read_gpReg64(state, operand.branchRegister.xn);
    write_PC(state, registerValue);
}

void unconditionalBranch(processorState_t *state, branchOperand_t operand) {
    LOG_DEBUG("uncond branch");
    increment_PC(state, operand.branchUnconditional.offset * 4);
}

bool evalCondition(processorState_t *state, uint32_t condition) {
    pState_t flags = read_pState(state);
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
    branchOperand_t operand = { .raw=instruction.operand };
    branchOperations[instruction.type](state, operand);
}

void executeLoadLiteral(processorState_t *state, loadLitInstruction_t instruction) {
    if (instruction.rt == STACK_POINTER)
        LOG_FATAL("SP not supported");

    uint64_t address = read_PC(state) + instruction.simm19 * 4;
    if (instruction.sf) {
        uint64_t data = read_mem64(state, address);
        write_gpReg64(state, instruction.rt, data);
    } else {
        uint32_t data = read_mem32(state, address);
        write_gpReg32(state, instruction.rt, data);
    } 
}

uint64_t computeOffset(processorState_t *state, SDTInstruction_t instruction) {
    SDTOffset_u offset_type = { .raw = instruction.offset };
    if (instruction.u) {
        if (instruction.sf) {
            return instruction.offset * 8;
        } else {
            return instruction.offset * 4;
        }
    } else if ((instruction.offset & REGISTER_OFFSET_MASK) == REGISTER_OFFSET_VALUE) {
        return read_gpReg64(state, offset_type.registerOffset.xm);
    } else if ((instruction.offset & PRE_POST_INDEX_MASK) == PRE_POST_INDEX_VALUE) {
        if (offset_type.prePostIndex.i) {
            return offset_type.prePostIndex.simm9;
        } else {
            return 0;
        }
    } else {
        LOG_FATAL("Offset type does not exist");
    }
}

void executeSDT(processorState_t *state, SDTInstruction_t instruction) {
    if (instruction.rt == STACK_POINTER || instruction.xn == STACK_POINTER)
        LOG_FATAL("SP not supported");

    uint64_t offset = computeOffset(state, instruction);
    uint64_t address = offset + read_gpReg64(state, instruction.xn);

    if (instruction.sf) {
        if (instruction.l) {
            uint64_t data = read_mem64(state, address);
            write_gpReg64(state, instruction.rt, data);
        } else {
            uint64_t data = read_gpReg64(state, instruction.rt);
            write_mem64(state, address, data);
        }
    } else {
        if (instruction.l) {
            uint32_t data = read_mem32(state, address);
            write_gpReg32(state, instruction.rt, data);
        } else {
            uint64_t data = read_gpReg32(state, instruction.rt);
            write_mem32(state, address, data);
        }
    }
    
    SDTOffset_u offset_type = { .raw = instruction.offset };
    if (!instruction.u && (instruction.offset & PRE_POST_INDEX_MASK) == PRE_POST_INDEX_VALUE) {
        if (offset_type.prePostIndex.i) {
            write_gpReg64(state, instruction.xn, address);
        } else {
            write_gpReg64(state, instruction.xn, address + offset_type.prePostIndex.simm9);
        }
    }
}