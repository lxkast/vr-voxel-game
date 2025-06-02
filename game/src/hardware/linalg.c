#include <logging.h>
#include <math.h>
#include "linalg.h"
/*
 * Assumes that the timestep is small
 */
void quat_fromEulers(double *rv, double dt, quaternion res) {
    double squaredSum = 0.0;
    for (int i = 0; i < 3; i++) {
        squaredSum += rv[i] * rv[i];
    }

    double m = sqrt(squaredSum);
    if (m == 0) {
        res[0] = 1;
        res[1] = 0;
        res[2] = 0;
        res[3] = 0;
        return;
    }
    res[0] = cos(m * dt / 2);
    res[1] = sin(m * dt / 2) * (rv[0] / m);
    res[2] = sin(m * dt / 2) * (rv[1] / m);
    res[3] = sin(m * dt / 2) * (rv[2] / m);
}

void quat_multiply(quaternion r, quaternion q, quaternion res) {
    if (r == res || q == res) {
        LOG_FATAL("Cannot multiply quaternions in place");
    }
    res[0] = r[0] * q[0] - r[1] * q[1] - r[2] * q[2] - r[3] * q[3];
    res[1] = r[0] * q[1] + r[1] * q[0] - r[2] * q[3] + r[3] * q[2];
    res[2] = r[0] * q[2] + r[1] * q[3] + r[2] * q[0] - r[3] * q[1];
    res[3] = r[0] * q[3] - r[1] * q[2] + r[2] * q[1] + r[3] * q[0];
}

/*
 * Multiplies two NxN matrices
 */
void matmul(double **a, double **b, const unsigned int size, double **res) {
    if (res == a || res == b) {
        LOG_FATAL("Cannot multiply matrices in place");
    }
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            double sum = 0.0;
            for (int z = 0; z < size; z++) {
                sum += a[x][z] * b[z][y];
            }
        }
    }
}
