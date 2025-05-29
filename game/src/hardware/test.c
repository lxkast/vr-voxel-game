#include <logging.h>
#include <stdint.h>
#include <stdio.h>

#include "icm42688.c"

void main(void) {
    log_init(stdout);
    LOG_DEBUG("Running");

    init_imu();

    while(1) {
        LOG_DEBUG("Temp reading %f", readTemp());
        if (isGyroEnabled()) {
            LOG_DEBUG("Gyro reading %f", readGyro());
        } else {
            LOG_DEBUG("Gyro not enabled");
        }
        sleep(1);
    }
} 
