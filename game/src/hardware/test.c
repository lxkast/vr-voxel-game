#include <logging.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "icm42688.h"
#include "quaternion.h"

double getCurrentTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1000000000.0;
}

void main(void) {
    log_init(stdout);
    static quaternion curr = {1, 0};
    LOG_DEBUG("Running");
    float start = getCurrentTime();
    init_imu();
    
    fifodata_t res;
    
    double lastTime = 0.0;

    while(1) {
        readFIFOData(&res);
        double deltaTime =  res.timestamp - lastTime;
        lastTime = res.timestamp;

        if (!res.accelValid) {
            LOG_FATAL("Invalid accel");
        }
        if (!res.gyroValid) {
            LOG_FATAL("INvalid gyro");
        } else { 
//        LOG_DEBUG("Current quaternion %f, %f, %f, %f", curr[0], curr[1], curr[2], curr[3]);        
        quaternion diff;
        fromEulers(res.gyro, deltaTime, diff);               
 
        quaternion qres;
        multiplyQuat(curr, diff, qres);

        memcpy(curr, qres, 4 * sizeof(double));

        
        LOG_DEBUG("Current quaternion %f, %f, %f, %f", curr[0], curr[1], curr[2], curr[3]);        
        }
    } 
} 
