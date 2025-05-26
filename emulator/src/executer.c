#include <assert.h>
#include <logging.h>
#include "executer.h"

#include "memory.h"

#define STACK_POINTER 0x1F

#define REGISTER_OFFSET_MASK 0x83F    // 2^11 + 2^5 + 2^4 + ... + 2^0
#define REGISTER_OFFSET_VALUE 0x81A   // 2^11 + 2^4 + 2^3 + 2^1

#define PRE_POST_INDEX_MASK 0x801     // 2^11 + 2^0
#define PRE_POST_INDEX_VALUE 0x1      // 2^0

#define OPI_ARITHMETIC 2
#define OPI_WIDE_MOVE 5

_Static_assert(sizeof(DPImmInstruction_t) == 4, "DPImmInstruction_t must be 4 bytes");
_Static_assert(sizeof(DPRegInstruction_t) == 4, "DPRegInstruction_t must be 4 bytes");
_Static_assert(sizeof(SDTInstruction_t) == 4, "SDTInstruction_t must be 4 bytes");
_Static_assert(sizeof(loadLitInstruction_t) == 4, "loadLitInstruction_t must be 4 bytes");
_Static_assert(sizeof(branchInstruction_t) == 4, "branchInstruction_t must be 4 bytes");
_Static_assert(sizeof(instruction_u) == 4, "instruction_u must be 4 bytes");

typedef void (*ArithmeticOperation)(processorState_t *state, DPImmInstruction_t instruction, arithmeticOperand_t operand);
typedef void (*WideMoveOperation)(processorState_t *state, DPImmInstruction_t instruction, wideMoveOperand_t operand);
typedef void (*LogicalOperation);
typedef void (*BranchOperation)(processorState_t *state, branchOperand_t operand);
typedef void (*RegOp64)(processorState_t *state, const reg_t dest, const uint64_t op1, const uint64_t op2);
typedef void (*RegOp32)(processorState_t *state, const reg_t dest, const uint32_t op1, const uint32_t op2);
typedef uint64_t (*BitwiseShift64)(uint64_t rm, uint64_t operand);
typedef uint32_t (*BitwiseShift32)(uint32_t rm, uint32_t operand);
typedef uint64_t (*LogicalOp64)(uint64_t op1, uint64_t op2);
typedef uint32_t (*LogicalOp32)(uint32_t op1, uint32_t op2);

void add64(processorState_t *state, const reg_t dest, const uint64_t op1, const uint64_t op2) {
    const uint64_t result = op1 + op2;
    write_reg64z(state, dest, result);
}

void add32(processorState_t *state, const reg_t dest, const uint32_t op1, const uint32_t op2) {
    const uint32_t result = op1 + op2;
    write_reg32z(state, dest, result);
}

void adds64(processorState_t *state, const reg_t dest, const uint64_t op1, const uint64_t op2) {
    const uint64_t result = op1 + op2;
    // Setting flags

    pState_t pState;
    pState.N = result >> 63;     // Negative flag
    pState.Z = result == 0;      // Zero flag
    pState.C = result < op1;     // Carry flag
    pState.V = (~((op1 ^ op2)) & (op1 ^ result)) >> 63;   // signed overflow/underflow flag

    write_pState(state, pState);
    write_reg64z(state, dest, result);
}

void adds32(processorState_t *state, const reg_t dest, const uint32_t op1, const uint32_t op2) {
    const uint32_t result = op1 + op2;

    // Setting flags
    pState_t pState;
    pState.N = result >> 31;     // Negative flag
    pState.Z = result == 0;      // Zero flag
    pState.C = result < op1;     // Carry flag
    pState.V = (~((op1 ^ op2)) & (op1 ^ result)) >> 31;   // signed overflow/underflow flag

    write_pState(state, pState);
    write_reg32z(state, dest, result);
}

void sub64(processorState_t *state, const reg_t dest, const uint64_t op1, const uint64_t op2) {
    const uint64_t result = op1 - op2;
    write_reg64z(state, dest, result);
}

void sub32(processorState_t *state, const reg_t dest, const uint32_t op1, const uint32_t op2) {
    const uint32_t result = op1 - op2;
    write_reg32z(state, dest, result);
}

void subs64(processorState_t *state, const reg_t dest, const uint64_t op1, const uint64_t op2) {
    const uint64_t result = op1 - op2;

    pState_t pState;
    pState.N = result >> 63;     // Negative flag
    pState.Z = result == 0;      // Zero flag
    pState.C = op1 >= op2;     // Carry flag
    pState.V = (((op1 ^ op2)) & (op1 ^ result)) >> 63;   // signed overflow/underflow flag

    write_pState(state, pState);
    write_reg64z(state, dest, result);
}

void subs32(processorState_t *state, const reg_t dest, const uint32_t op1, const uint32_t op2) {
    const uint32_t result = op1 - op2;

    // Setting flags
    pState_t pState;
    pState.N = result >> 31;     // Negative flag
    pState.Z = result == 0;      // Zero flag
    pState.C = op1 >= op2;        // Carry flag
    pState.V = (((op1 ^ op2)) & (op1 ^ result)) >> 31;   // signed overflow/underflow flag

    write_pState(state, pState);
    write_reg32z(state, dest, result);
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

BitwiseShift32 bitwiseShift32[] = {
    lsl32,
    lsr32,
    asr32,
    ror32,
};

BitwiseShift64 bitwiseShift64[] = {
    lsl64,
    lsr64,
    asr64,
    ror64,
};

void executeAdd(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    const uint64_t op2 = ((uint64_t) operand.imm12) << (operand.sh * 12);
    if (instruction.sf) {
       add64(state, instruction.rd, read_reg64z(state, operand.rn), op2);
    } else {
        // op2 is guaranteed to be <= 32 bits, so casting it down to uint32_t is fine
        add32(state, instruction.rd, read_reg32z(state, operand.rn), op2);
    }
}

// performs the add operation, storing values in flags
void executeAdds(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    const uint64_t op2 = ((uint64_t) operand.imm12) << (operand.sh * 12);
    if (instruction.sf) {
        const uint64_t rn = read_reg64z(state, operand.rn);
        adds64(state, instruction.rd, rn, op2);
    } else {
        const uint32_t rn = read_reg32z(state, operand.rn);
        adds32(state, instruction.rd, rn, op2);
    }
}

void executeSub(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    const uint64_t op2 = ((uint64_t) operand.imm12) << (operand.sh * 12);
    if (instruction.sf) {
        sub64(state, instruction.rd, read_reg64z(state, operand.rn), op2);
    } else {
        // op2 is guaranteed to be <= 32 bits, so casting it down to uint32_t is fine
        sub32(state, instruction.rd, read_reg32z(state, operand.rn), op2);
    }
}

// performs the sub operation, storing values in flags
void executeSubs(processorState_t *state, const DPImmInstruction_t instruction, const arithmeticOperand_t operand) {
    const uint64_t op2 = ((uint64_t) operand.imm12) << (operand.sh * 12);
    if (instruction.sf) {
        const uint64_t rn = read_reg64z(state, operand.rn);
        subs64(state, instruction.rd, rn, op2);
    } else {
        const uint32_t rn = read_reg32z(state, operand.rn);
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

void executeAnd(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitwiseShift64[opr.shift](read_reg64z(state,instruction.rm),instruction.operand);
        const uint64_t result = read_reg64z(state,instruction.rn) & op2;
        write_reg64z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitwiseShift32[opr.shift](read_reg32z(state,instruction.rm),instruction.operand);
        const uint32_t result = read_reg32z(state,instruction.rn) & op2;
        write_reg32z(state, instruction.rd, result);
    }
}
void executeBic(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitwiseShift64[opr.shift](read_reg64z(state,instruction.rm),instruction.operand);
        const uint64_t result = read_reg64z(state,instruction.rn) & ~op2;
        write_reg64z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitwiseShift32[opr.shift](read_reg32z(state,instruction.rm),instruction.operand);
        const uint32_t result = read_reg32z(state,instruction.rn) & ~op2;
        write_reg32z(state, instruction.rd, result);
    }
}
void executeOrr(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitwiseShift64[opr.shift](read_reg64z(state,instruction.rm),instruction.operand);
        const uint64_t result = read_reg64z(state,instruction.rn) | op2;
        write_reg64z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitwiseShift32[opr.shift](read_reg32z(state,instruction.rm),instruction.operand);
        const uint32_t result = read_reg32z(state,instruction.rn) | op2;
        write_reg32z(state, instruction.rd, result);
    }
}
void executeOrn(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitwiseShift64[opr.shift](read_reg64z(state,instruction.rm),instruction.operand);
        const uint64_t result = read_reg64z(state,instruction.rn) | ~op2;
        write_reg64z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitwiseShift32[opr.shift](read_reg32z(state,instruction.rm),instruction.operand);
        const uint32_t result = read_reg32z(state,instruction.rn) | ~op2;
        write_reg32z(state, instruction.rd, result);
    }
}
void executeEor(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitwiseShift64[opr.shift](read_reg64z(state,instruction.rm),instruction.operand);
        const uint64_t result = read_reg64z(state,instruction.rn) ^ op2;
        write_reg64z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitwiseShift32[opr.shift](read_reg32z(state,instruction.rm),instruction.operand);
        const uint32_t result = read_reg32z(state,instruction.rn) ^ op2;
        write_reg32z(state, instruction.rd, result);
    }
}
void executeEon(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitwiseShift64[opr.shift](read_reg64z(state,instruction.rm),instruction.operand);
        const uint64_t result = read_reg64z(state,instruction.rn) ^ ~op2;
        write_reg64z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitwiseShift32[opr.shift](read_reg32z(state,instruction.rm),instruction.operand);
        const uint32_t result = read_reg32z(state,instruction.rn) ^ ~op2;
        write_reg32z(state, instruction.rd, result);
    }
}
void executeAnds(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitwiseShift64[opr.shift](read_reg64z(state,instruction.rm),instruction.operand);
        const uint64_t result = read_reg64z(state,instruction.rn) & op2;

        pState_t pState;
        pState.N = result >> 63;
        pState.Z = result == 0;
        pState.C = 0;
        pState.V = 0;

        write_pState(state, pState);

        write_reg64z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitwiseShift32[opr.shift](read_reg32z(state,instruction.rm),instruction.operand);
        const uint32_t result = read_reg32z(state,instruction.rn) & op2;

        pState_t pState;
        pState.N = result >> 31;
        pState.Z = result == 0;
        pState.C = 0;
        pState.V = 0;

        write_pState(state, pState);

        write_reg32z(state, instruction.rd, result);
    }
}
void executeBics(processorState_t *state, const DPRegInstruction_t instruction, const logicalOpr_t opr) {
    if (instruction.sf) {
        const uint64_t op2 = bitwiseShift64[opr.shift](read_reg64z(state,instruction.rm),instruction.operand);
        const uint64_t result = read_reg64z(state,instruction.rn) & ~op2;

        pState_t pState;
        pState.N = result >> 63;
        pState.Z = result == 0;
        pState.C = 0;
        pState.V = 0;

        write_pState(state, pState);

        write_reg64z(state, instruction.rd, result);
    } else {
        const uint32_t op2 = bitwiseShift32[opr.shift](read_reg32z(state,instruction.rm),instruction.operand);
        const uint32_t result = read_reg32z(state,instruction.rn) & ~op2;

        pState_t pState;
        pState.N = result >> 31;
        pState.Z = result == 0;
        pState.C = 0;
        pState.V = 0;

        write_pState(state, pState);

        write_reg32z(state, instruction.rd, result);
    }
}

uint32_t and32(uint32_t op1, uint32_t op2) {
    return op1 & op2;
}

uint32_t orr32(uint32_t op1, uint32_t op2) {
    return op1 | op2;
}

uint32_t eor32(uint32_t op1, uint32_t op2) {
    return op1 ^ op2;
}

uint64_t and64(uint64_t op1, uint64_t op2) {
    return op1 & op2;
}

uint64_t orr64(uint64_t op1, uint64_t op2) {
    return op1 | op2;
}

uint64_t eor64(uint64_t op1, uint64_t op2) {
    return op1 ^ op2;
}

LogicalOp32 logicalOperations32[] = {
    and32,
    orr32,
    eor32,
    and32,
};

LogicalOp64 logicalOperations64[] = {
    and64,
    orr64,
    eor64,
    and64,
};

RegOp64 registerOperations64[] = {
    add64,
    adds64,
    sub64,
    subs64
};

RegOp32 registerOperations32[] = {
    add32,
    adds32,
    sub32,
    subs32
};

void executeMultiply(processorState_t *state, DPRegInstruction_t instruction) {
    const DPRegOperand_u operand = { .raw = instruction.operand };

    if (instruction.sf) {
        const uint64_t raV = read_reg64z(state, operand.multiply.ra);
        const uint64_t rmV = read_reg64z(state, instruction.rm);
        const uint64_t rnV = read_reg64z(state, instruction.rn);

        if (operand.multiply.x == 0) {
            write_reg64z(state, instruction.rd, raV + rmV * rnV);
        } else {
            write_reg64z(state, instruction.rd, raV - rmV * rnV);
        }
    } else {
        const uint32_t raV = read_reg32z(state, operand.multiply.ra);
        const uint32_t rmV = read_reg32z(state, instruction.rm);
        const uint32_t rnV = read_reg32z(state, instruction.rn);

        if (operand.multiply.x == 0) {
            write_reg32z(state, instruction.rd, raV + rmV * rnV);
        } else {
            write_reg32z(state, instruction.rd, raV - rmV * rnV);
        }
    }
}

void executeDPReg(processorState_t *state, const DPRegInstruction_t instruction) {
    const DPRegOpr_u DPopr = {.raw = instruction.opr};
    if (instruction.m == 0 && (instruction.opr | 0x6) == 0xE) {
        // arithmetic instruction
        const arithmeticOpr_t opr = DPopr.arithmetic;
        if (instruction.sf) {
            const uint64_t rn = read_reg64z(state,instruction.rn);
            const uint64_t rm = read_reg64z(state,instruction.rm);
            const uint64_t op2 = bitwiseShift64[opr.shift](rm, instruction.operand);
            registerOperations64[instruction.opc](state, instruction.rd,  rn, op2);
        } else {
            const uint64_t rn = read_reg32z(state,instruction.rn);
            const uint64_t rm = read_reg32z(state,instruction.rm);
            const uint64_t op2 = bitwiseShift32[opr.shift](rm, instruction.operand);
            registerOperations32[instruction.opc](state, instruction.rd,  rn, op2);
        }
    } else if (instruction.m == 0 && (instruction.opr | 0x7) == 0x7) {
        const logicalOpr_t opr = DPopr.logical;
        //logicalOperations[(instruction.opc << 1) + opr.N](state, instruction, opr);
        if (instruction.sf) {
            const uint64_t op2 = bitwiseShift64[opr.shift](read_reg64z(state,instruction.rm),instruction.operand);
            const uint64_t result = logicalOperations64[instruction.opc](read_gpReg64(state,instruction.rn), (opr.N? ~op2 : op2));
            if (instruction.opc == 3) {
                pState_t pState = {
                    .N = result >> 63,
                    .Z = result == 0,
                };
                write_pState(state, pState);
            }
            write_reg64z(state, instruction.rd, result);
        } else {
            const uint32_t op2 = bitwiseShift32[opr.shift](read_reg32z(state,instruction.rm),instruction.operand);
            const uint32_t result = logicalOperations32[instruction.opc](read_gpReg32(state,instruction.rn), (opr.N? ~op2 : op2));
            if (instruction.opc == 3) {
                pState_t pState = {
                    .N = result >> 31,
                    .Z = result == 0,
                };
                write_pState(state, pState);
            }
            write_reg32z(state, instruction.rd, result);
        }
    } else if (instruction.m && instruction.opr == 0x8) {
        executeMultiply(state, instruction);
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
    const pState_t flags = read_pState(state);
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

void conditionalBranch(processorState_t *state, const branchOperand_t operand) {
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

void executeLoadLiteral(processorState_t *state, const loadLitInstruction_t instruction) {
    if (instruction.rt == STACK_POINTER)
        LOG_FATAL("SP not supported");

    const uint64_t address = read_PC(state) + instruction.simm19 * 4;
    if (instruction.sf) {
        const uint64_t data = read_mem64(state, address);
        write_gpReg64(state, instruction.rt, data);
    } else {
        const uint32_t data = read_mem32(state, address);
        write_gpReg32(state, instruction.rt, data);
    } 
}

uint64_t computeOffset(processorState_t *state, const SDTInstruction_t instruction) {
    const SDTOffset_u offset_type = { .raw = instruction.offset };
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

void executeSDT(processorState_t *state, const SDTInstruction_t instruction) {
    if (instruction.rt == STACK_POINTER || instruction.xn == STACK_POINTER)
        LOG_FATAL("SP not supported");

    const uint64_t offset = computeOffset(state, instruction);
    const uint64_t address = offset + read_gpReg64(state, instruction.xn);

    if (instruction.sf) {
        if (instruction.l) {
            const uint64_t data = read_mem64(state, address);
            write_gpReg64(state, instruction.rt, data);
        } else {
            const uint64_t data = read_gpReg64(state, instruction.rt);
            write_mem64(state, address, data);
        }
    } else {
        if (instruction.l) {
            const uint32_t data = read_mem32(state, address);
            write_gpReg32(state, instruction.rt, data);
        } else {
            const uint64_t data = read_gpReg32(state, instruction.rt);
            write_mem32(state, address, data);
        }
    }
    
    const SDTOffset_u offset_type = { .raw = instruction.offset };
    if (!instruction.u && (instruction.offset & PRE_POST_INDEX_MASK) == PRE_POST_INDEX_VALUE) {
        if (offset_type.prePostIndex.i) {
            write_gpReg64(state, instruction.xn, address);
        } else {
            write_gpReg64(state, instruction.xn, address + offset_type.prePostIndex.simm9);
        }
    }
}
