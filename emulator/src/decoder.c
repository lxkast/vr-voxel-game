#include "decoder.h"

#include "executer.h"
#include "logging.h"
/*
    Defining masks, expected values to be used in later functions
*/

#define DP_IMM_MASK 0x1c000000        // 2^28 + 2^27 + 2^26
#define DP_IMM_VALUE 0x10000000       // 2^28

#define DP_REG_MASK 0xe000000         // 2^27 + 2^26 + 2^25
#define DP_REG_VALUE 0xa000000        // 2^27 + 2^25

#define SDT_MASK 0x8a000000           // 2^31 + 2^27 + 2^25
#define SDT_VALUE 0x88000000          // 2^31 + 2^27

#define LOAD_LIT_MASK 0x8a000000      // 2^31 + 2^27 + 2^25
#define LOAD_LIT_VALUE 0x8000000      // 2^27

#define BRANCH_MASK 0x1c000000        // 2^28 + 2^27 + 2^26
#define BRANCH_VALUE 0x14000000       // 2^28 + 2^26



/*
    Takes an instruction and determines what type it is, then uses a subsequent
    function to execute the instruction, depending on type. Checks for the HALT
    function and throws a fatal error if no matches are found.

    Modifies the program counter
*/
bool decodeAndExecute(processorState_t *state, const uint32_t rawInstruction) {
    const instruction_u instruction = { .raw = rawInstruction };
    if (rawInstruction == 0x8a000000) {
        return false;
    }

    if ((rawInstruction & DP_IMM_MASK) == DP_IMM_VALUE) {
        executeDPImm(state, instruction.dpimm);
    } else if ((rawInstruction & DP_REG_MASK) == DP_REG_VALUE) {
        // TODO: execute instruction
    } else if ((rawInstruction & SDT_MASK) == SDT_VALUE) {
        // TODO: execute instruction
    } else if ((rawInstruction & LOAD_LIT_MASK) == LOAD_LIT_VALUE) {
        // TODO: execute instruction
    } else if ((rawInstruction & BRANCH_MASK) == BRANCH_VALUE) {
        executeBranch(state, instruction.branch);
        return true;
    } else {
        LOG_FATAL("Unhandled instruction type");
    }
    increment_PC(state, 4);
    
    return true;
}
