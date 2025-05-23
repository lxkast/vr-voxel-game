#include <stdint.h>

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
    Creating structs for all the different types of instruction
*/

typedef struct {
    uint8_t sf : 1;
    uint8_t opc : 2;
    uint8_t : 3;
    uint8_t opi : 3;
    uint32_t operand : 18;
    uint8_t rd : 5;
} DPImmInstruction_t;

typedef struct {
    uint8_t sf : 1;
    uint8_t opc : 2;
    uint8_t m : 1;
    uint8_t : 3;
    uint8_t opr : 4;
    uint8_t rm : 5;
    uint8_t operand : 6;
    uint8_t rn : 5;
    uint8_t rd : 5;
} DPRegInstruction_t;

typedef struct {
    uint8_t : 1;
    uint8_t sf : 1;
    uint8_t : 7;
    uint8_t l : 1;
    uint16_t offset : 12;
    uint8_t xn : 5;
    uint8_t rt : 5;
} SDTInstruction_t;

typedef struct {
    uint8_t : 1;
    uint8_t sf : 1;
    uint8_t : 6;
    uint16_t simm19 : 19;
    uint8_t rt : 5;
} loadLitInstruction_t;

typedef struct {
    uint8_t type : 2;
    uint8_t : 4;
    uint32_t operand : 26;
} branchInstruction_t;

/*
    Takes an instruction and determines what type it is, then uses a subsequent
    function to execute the instruction, depending on type. Checks for the HALT
    function and throws a fatal error if no matches are found
*/
void decodeAndExecute(const int instruction) {
    if (instruction == 0x8a000000) {
        // TODO: HALT instruction
    } else if ((instruction & DP_IMM_MASK) == DP_IMM_VALUE) {
        DPImmInstruction_t decoded = *(DPImmInstruction_t*)&instruction;
        executeDPIMM(decoded);
        // TODO: execute instruction
    } else if ((instruction & DP_REG_MASK) == DP_REG_VALUE) {
        DPRegInstruction_t decoded = *(DPRegInstruction_t*)&instruction;
        // TODO: execute instruction
    } else if ((instruction & SDT_MASK) == SDT_VALUE) {
        SDTInstruction_t decoded = *(SDTInstruction_t*)&instruction;
        // TODO: execute instruction
    } else if ((instruction & LOAD_LIT_MASK) == LOAD_LIT_VALUE) {
        loadLitInstruction_t decoded = *(loadLitInstruction_t*)&instruction;
        // TODO: execute instruction
    } else if ((instruction & BRANCH_MASK) == BRANCH_VALUE) {
        branchInstruction_t decoded = *(branchInstruction_t*)&instruction;
        // TODO: execute instruction
    } else {
        LOG_FATAL("Unhandled instruction type");
    }
}