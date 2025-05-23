#include <stdlib.h>
#include <logging.h>

int main(int argc, char **argv) {
    log_init(stdout);
    LOG_INFO("Emulator started.");

    return EXIT_SUCCESS;
}

