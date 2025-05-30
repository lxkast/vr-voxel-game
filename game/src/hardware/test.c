#include <logging.h>
#include <stdint.h>
#include <stdio.h>

#include "icm42688.h"

void main(void) {
    log_init(stdout);
    LOG_DEBUG("Running");

    init_imu();

    while(1) {
        readFIFOData();
        usleep(0);
    } 
} 
