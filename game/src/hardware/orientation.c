#include <logging.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "icm42688.h"
#include "orientation.h"

typedef double state_t[4];

static pthread_t thread;

static state_t currentState = {1, 0, 0, 0};

static double accelSensorToLocal[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, -1}};
static double rotationSensorToLocal[3][3] = {{-1, 0, 0}, {0, -1, 0}, {0, 0, 1}};

static double gravityDir[3] = {0, -1, 0};

static double P[4][4] = {{0.01, 0, 0, 0}, {0, 0.01, 0, 0}, {0, 0, 0.01, 0}, {0, 0, 0, 0.01}};
static double Q[4][4] =  {
    {1e-6,  0,     0,     0    },
    {0,     1e-4,  0,     0    },
    {0,     0,     1e-4,  0    },
    {0,     0,     0,     1e-4 }
};


static double R[3][3] =  {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1}
};



static const double I[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

static void orientation_init(double accels[3]) {
    // first normalise accels
    vec_normalise(accels, 3, accels);
    
    double angle = acos(dotProduct3(accels, gravityDir));
    currentState[0] = cos(angle/2);
    crossProduct3(gravityDir, accels, currentState+1);
    currentState[1] *= sin(angle/2);
    currentState[2] *= sin(angle/2);
    currentState[3] *= sin(angle/2);
    quat_normalise(currentState, currentState);
 //   quat_conjugate(currentState, currentState);
}

void imu_getOrientation(quaternion res) {
    memcpy(res, currentState, 4 * sizeof(double));
}

static void predict(double dt, double gyro[3]) {
    quaternion diff;
    quat_fromEulers(gyro, dt, diff);
    quaternion res;
    quat_multiply(currentState, diff, res);
    memcpy(currentState, res, 4 * sizeof(double));

    // update covariance this is jacobian
    double F[4][4] = {
        {diff[0], -diff[1], -diff[2], -diff[3]},
        {diff[1], diff[0], -diff[3], diff[2]},
        {diff[2], diff[3], diff[0], -diff[1]},
        {diff[3], -diff[2], diff[1], diff[0]}};

    double Ft[4][4];
    mat_transpose(F, 4, 4, Ft);

    double temp[4][4];
    matmul(F, P, 4, 4, 4, 4, temp);
    matmul(temp, Ft, 4, 4, 4, 4, P);
    mat_add(P, Q, 4, 4, P);
}

static void update(double accels[3]) {
    vec_normalise(accels, 3, accels);
    // predict measurements
    double predicted[3];
    quat_vecmul(currentState, gravityDir, predicted);

    quaternion inv;
    quat_conjugate(currentState, inv);
    double other[3];
    quat_vecmul(inv, gravityDir, predicted);
 
    double yk[3] = {accels[0] - predicted[0], accels[1] - predicted[1], accels[2] - predicted[2]};
    double H[3][4] = {
        {-currentState[3] * 2, -currentState[2] * 2, -currentState[1] * 2, -currentState[0] * 2},
        {currentState[0] * -2, -currentState[1] * -2, -currentState[2] * 2, -currentState[3] * 2},
        {2 * currentState[1], 2 * currentState[0], -2 * currentState[3], 2 * -currentState[2]}
    };

    double temp[3][4];
    double Ht[4][3];
    mat_transpose(H, 3, 4, Ht);

    matmul(H, P, 3, 4, 4, 4, temp);
    double S[3][3];
    matmul(temp, Ht, 3, 4, 4, 3, S);
    mat_add(S, R, 3, 3, S);

    double SI[3][3];
    inverse3x3(S, SI);

    double temp2[4][3];
    matmul(P, Ht, 4, 4, 4, 3, temp2);
    double K[4][3];
    matmul(temp2, SI, 4, 3, 3, 3, K);

    double update[4];
    matmul(K, yk, 4, 3, 3, 1, update);
    mat_add(update, currentState, 4, 1, currentState);

    double temp3[4][4];
    matmul(K, H, 4, 3, 3, 4, temp3);

    mat_sub(I, temp3, 4, 4, temp3);
    double temp4[4][4];
    matmul(temp3, P, 4, 4, 4, 4, temp4);
    memcpy(P, temp4, 4 * 4 * sizeof(double));

    (void) quat_normalise(currentState, currentState);
    // mat_divs(P, 4, 4, mag * mag, P);
}

void *runOrientation(void *arg) {
    LOG_DEBUG("Running orientation thread");
    init_imu();

    fifodata_t res;
    FILE *fpt = fopen("output.csv", "w+");

    double lastTime = 0.0;
    bool init = false;

    while (1) {
        usleep(10);
        if (readFIFOData(&res)) {
            double deltaTime = res.timestamp - lastTime;
            lastTime = res.timestamp;

            if (res.gyroValid && init) {

                double gyroLocal[3];
                matmul(rotationSensorToLocal, res.gyro, 3, 3, 3, 1, gyroLocal);
                predict(deltaTime, gyroLocal);
            }

            if (res.accelValid) {
                if (!res.gyroValid) LOG_DEBUG("Accel valid, but gyro isn't, out of sync, help");

                double accelsLocal[3];
                matmul(accelSensorToLocal, res.accel, 3, 3, 3, 1, accelsLocal);
                if (!init) {
                    orientation_init(accelsLocal);
                    init = true;
                } else {
                    update(accelsLocal);
                }
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
