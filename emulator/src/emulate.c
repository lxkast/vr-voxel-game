#include <stdlib.h>
#include <logging.h>
#include <futil.h>
#include <string.h>
#include "state.h"
#include "memory.h"
#include "decoder.h"

void runProgram(processorState_t *state) {
    uint32_t instruction;
    do {
        // fetch
        instruction = read_mem32(state, read_PC(state));
        LOG_INFO("Executing instruction 0x%x", instruction);
    } while (decodeAndExecute(state, instruction));
}

void writeOutput(processorState_t *state, FILE *file) {
    fprintf(file, "Registers:\n");
    for (int i = 0; i < 31; i++) {
        fprintf(file, "X%02d = %016lx \n", i, read_gpReg64(state, i));
    }
    fprintf(file, "PC = %016lx\n", read_PC(state));

    fprintf(file, "PSTATE : ");
    pState_t pState = read_pState(state);
    if (pState.N) {
        fprintf(file, "N");
    } else {
        fprintf(file, "-");
    }
    if (pState.Z) {
        fprintf(file, "Z");
    } else {
        fprintf(file, "-");
    }
    if (pState.C) {
        fprintf(file, "C");
    } else {
        fprintf(file, "-");
    }
    if (pState.V) {
        fprintf(file, "V");
    } else {
        fprintf(file, "-");
    }

    fprintf(file, "\nNon-zero memory:\n");
    write_memory(state, file);
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
    free(programInstructions);

    runProgram(state);

    FILE *outputFile;
    if (argc == 3) {
        outputFile = fopen(argv[2], "w");
    } else {
        outputFile = stdout;
    }

    writeOutput(state, outputFile);

    freeState(state);

    futil_close(fp);
    return EXIT_SUCCESS;
}
