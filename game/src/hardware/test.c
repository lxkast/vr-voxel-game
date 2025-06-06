#include <logging.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "icm42688.h"
#include "linalg.h"
#include "orientation.h"

void main(void) {
    log_init(stdout);
    LOG_DEBUG("Running");
    startOrientationThread();
    quaternion orientation;

    while(true) {
        imu_getOrientation(orientation);
        LOG_DEBUG("Orienation is %f %f %f %f", orientation[0], orientation[1], orientation[2], orientation[3]);
    sleep(1);
    }    
} 
