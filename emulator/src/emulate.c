#include <stdlib.h>
#include <logging.h>
#include <futil.h>
#include <string.h>

#include "state.h"
#include "memory.h"
#include "decoder.h"

// TODO think of a better name
void mainLoop(processorState_t *state) {
    uint32_t instruction;
    do {
        // fetch
        instruction = read_mem32(state, state->spRegisters.PC);
    } while (decodeAndExecute(state, instruction));
}

void write(processorState_t *state, FILE *file) {
    fprintf(file, "Registers:\n");
    for (int i = 0; i < 31; i++) {
        fprintf(file, "X%02d = %016lx \n", i, read_gpReg64(state, i));
    }
    fprintf(file, "PC = %016lx\n", state->spRegisters.PC);

    fprintf(file, "PSTATE : ");
    if (state->spRegisters.PSTATE.N) {
        fprintf(file, "N");
    } else {
        fprintf(file, "-");
    }
    if (state->spRegisters.PSTATE.Z) {
        fprintf(file, "Z");
    } else {
        fprintf(file, "-");
    }
    if (state->spRegisters.PSTATE.C) {
        fprintf(file, "C");
    } else {
        fprintf(file, "-");
    }
    if (state->spRegisters.PSTATE.V) {
        fprintf(file, "V");
    } else {
        fprintf(file, "-");
    }

    fprintf(file, "\nNon-zero memory:\n");
    for (int i = 0; i < MEMORY_SIZE; i+=4) {
        const uint32_t value = read_mem32(state, i);
        if (value) {
            fprintf(file, "0x%08X : %08x\n", i, value);
        }
    }
}

int main(int argc, char **argv) {
    log_init(stdout);
    if (argc < 2) {
        LOG_INFO("Usage: emulate <file_in> <file_out>");
    }
    char* fileIn = argv[1];
    futil_file_t *fp = futil_open(fileIn, FUTIL_FILE_READ);
    if (!fp) {
        LOG_FATAL("Failed to open file %s", fileIn);
    }
    int numInstructions = futil_fileSize(fp);
    uint32_t *programInstructions = malloc(numInstructions * sizeof(uint32_t));
    futil_readBinary(fp, programInstructions, numInstructions);

    processorState_t *state = malloc(sizeof(processorState_t));
    initState(state, programInstructions, numInstructions);

    mainLoop(state);

    FILE *outputFile;
    if (argc == 3) {
        outputFile = fopen(argv[2], "w");
    } else {
        outputFile = stdout;
    }

    write(state, outputFile);

    free (programInstructions);
    free(state);

    futil_close(fp);
    return EXIT_SUCCESS;
}
