#include <stdlib.h>
#include <logging.h>
#include <futil.h>
#include <string.h>

#include "state.h"
#include "memory.h"
#include "decoder.h"

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



    free (programInstructions);
    free(state);

    futil_close(fp);
    return EXIT_SUCCESS;
}

// TODO think of a better name
void mainLoop(processorState_t *state) {
    uint32_t instruction;
    do {
        // fetch
        instruction = read_mem32(state, state->spRegisters.PC);
    } while (decodeAndExecute(instruction));
}