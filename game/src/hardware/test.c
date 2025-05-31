#include <logging.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "icm42688.h"

double getCurrentTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1000000000.0;
}

void main(void) {
    log_init(stdout);
    LOG_DEBUG("Running");
    float start = getCurrentTime();
    init_imu();
    
    fifodata_t res;
    
    while(1) {
        readFIFOData(&res);
        float error =  res.timestamp - (getCurrentTime() - start);   
        LOG_DEBUG("Current error: %f, percentage: %f%", error, error * 100 / res.timestamp );
        
    } 
} 
