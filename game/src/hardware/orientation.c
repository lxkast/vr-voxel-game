#include <string.h>
#include <logging.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>

#include "icm42688.h"
#include "quaternion.h"

#include "orientation.h"

typedef struct {
    quaternion ori;
    double[3] omega;
} state_t;

pthread_t thread;

state_t currentState = {.ori = {1, 0}, .omega = {0}}

void imu_getOrientation(quaternion res) {
    memcpy(res, orientation, 4 * sizeof(double));
}

void predict() {
    
}

void* runOrientation(void* arg) {
    LOG_DEBUG("Running orientation thread");
    init_imu();
    
    fifodata_t res;
    FILE *fpt = fopen("output.csv", "w+");

    double lastTime = 0.0;

    while(1) {
        if (readFIFOData(&res)) {
            double deltaTime =  res.timestamp - lastTime;
            lastTime = res.timestamp;

            if (!res.accelValid) {
                LOG_FATAL("Invalid accel");
            }
            if (!res.gyroValid) {
                LOG_FATAL("INvalid gyro");
                continue;
            } else {
                fprintf(fpt, "%f, %f, %f\n", res.gyro[0], res.gyro[1], res.gyro[2]);
                quaternion diff;
                fromEulers(res.gyro, deltaTime, diff);               
 
                quaternion qres;
                multiplyQuat(orientation, diff, qres);

                memcpy(orientation, qres, 4 * sizeof(double));

        
                //LOG_DEBUG("Current quaternion %f, %f, %f, %f", orientation[0], orientation[1], orientation[2], orientation[3]);        
            }
        }
    }
}

void cleanup(void) {
    pthread_cancel(thread);
    pthread_join(thread, NULL);
}

void startOrientationThread() {
    if (pthread_create(&thread, NULL, runOrientation, NULL) != 0) {
        LOG_FATAL("Failed to create thread");
        return;
    }
    atexit(cleanup);
}
