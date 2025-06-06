#include <logging.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "icm42688.h"
#include "orientation.h"

typedef double state_t[4];

static pthread_t thread;

static state_t currentState = {1, 0, 0, 0};

void orientation_init(double accels[3]) {
    static double expected[3] = {0, 0, -1};
    // first normalise accels
    vec_normalise(accels, 3, accels);
    double dot = dotProduct3(accels, expected);
    currentState[0] = dot;
    crossProduct3(expected, accels, currentState+1);
    quat_normalise(currentState, currentState);
}

void imu_getOrientation(quaternion res) {
    memcpy(res, currentState, 4 * sizeof(double));
}

void predict(double dt) {
    quaternion diff;
    quat_fromEulers(&currentState[4], dt, diff);
    quaternion res;
    quat_multiply(currentState, diff, res);
    memcpy(currentState, res, 4 * sizeof(double));
}

void *runOrientation(void *arg) {
    LOG_DEBUG("Running orientation thread");
    init_imu();

    fifodata_t res;
    FILE *fpt = fopen("output.csv", "w+");

    double lastTime = 0.0;

    while (1) {
        usleep(10);
        if (readFIFOData(&res)) {
            double deltaTime = res.timestamp - lastTime;
            lastTime = res.timestamp;

            if (!res.accelValid) {
                LOG_FATAL("Invalid accel");
            }
            if (!res.gyroValid) {
                LOG_FATAL("INvalid gyro");
                continue;
            } else {
                // fprintf(fpt, "%f, %f, %f\n", res.gyro[0], res.gyro[1], res.gyro[2]);
                // quaternion diff;
                // quat_fromEulers(res.gyro, deltaTime, diff);
                //
                // quaternion qres;
                // quat_multiply(currentState, diff, qres);
                //
                // memcpy(currentState, qres, 4 * sizeof(double));
                orientation_init(res.accel);

                // LOG_DEBUG("Current quaternion %f, %f, %f, %f", orientation[0], orientation[1], orientation[2], orientation[3]);
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
