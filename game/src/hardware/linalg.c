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
 * Note this function can be used in place.
 */
void quat_normalise(quaternion r, quaternion res) {
    double magnitude = sqrt(r[0] * r[0] + r[1] * r[1] + r[2] * r[2] + r[3] * r[3]);
    if (magnitude == 0) { // this is a bit of an issue, return unit quaternion
        res[0] = 1;
        res[1] = 0;
        res[2] = 0;
        res[3] = 0;
    } else {
        res[0] = r[0] / magnitude;
        res[1] = r[1] / magnitude;
        res[2] = r[2] / magnitude;
        res[3] = r[3] / magnitude;
    }
}

/*
 * Multiplies two NxN matrices
 */
void matmul(double *a, double *b, const unsigned int size, double *res) {
    if (res == a || res == b) {
        LOG_FATAL("Cannot multiply matrices in place");
    }
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            double sum = 0.0;
            for (int z = 0; z < size; z++) {
                sum += a[x* size + z] * b[z* size + y];
            }
        }
    }
}

/*
 *  Normalises a vector of length N, returns all zeros if magnitude of vector is 0
 */
void vec_normalise(double *r, int n, double *res) {
    double magnitude = 0.0;
    for (int i = 0; i < n; i++) {
        magnitude += r[i] * r[i];
    }
    magnitude = sqrt(magnitude);

    if (magnitude == 0) { // this is a bit of an issue, return unit quaternion
        for (int i = 0; i < n; i++) {
            res[i] = 0;
        }
    } else {
        for (int i = 0; i < n; i++) {
            res[i] = r[i] / magnitude;
        }
    }
}


// NOTE: function written by script matrix_inverse.py, TODO make this more efficient
void inverse4x4(double a[4][4], double res[4][4]) {
    double det = a[0][0]*a[1][1]*a[2][2]*a[3][3] - a[0][0]*a[1][1]*a[2][3]*a[3][2] - a[0][0]*a[1][2]*a[2][1]*a[3][3] + a[0][0]*a[1][2]*a[2][3]*a[3][1] + a[0][0]*a[1][3]*a[2][1]*a[3][2] - a[0][0]*a[1][3]*a[2][2]*a[3][1] - a[0][1]*a[1][0]*a[2][2]*a[3][3] + a[0][1]*a[1][0]*a[2][3]*a[3][2] + a[0][1]*a[1][2]*a[2][0]*a[3][3] - a[0][1]*a[1][2]*a[2][3]*a[3][0] - a[0][1]*a[1][3]*a[2][0]*a[3][2] + a[0][1]*a[1][3]*a[2][2]*a[3][0] + a[0][2]*a[1][0]*a[2][1]*a[3][3] - a[0][2]*a[1][0]*a[2][3]*a[3][1] - a[0][2]*a[1][1]*a[2][0]*a[3][3] + a[0][2]*a[1][1]*a[2][3]*a[3][0] + a[0][2]*a[1][3]*a[2][0]*a[3][1] - a[0][2]*a[1][3]*a[2][1]*a[3][0] - a[0][3]*a[1][0]*a[2][1]*a[3][2] + a[0][3]*a[1][0]*a[2][2]*a[3][1] + a[0][3]*a[1][1]*a[2][0]*a[3][2] - a[0][3]*a[1][1]*a[2][2]*a[3][0] - a[0][3]*a[1][2]*a[2][0]*a[3][1] + a[0][3]*a[1][2]*a[2][1]*a[3][0];
    res[0][0] = (a[1][1]*a[2][2]*a[3][3] - a[1][1]*a[2][3]*a[3][2] - a[1][2]*a[2][1]*a[3][3] + a[1][2]*a[2][3]*a[3][1] + a[1][3]*a[2][1]*a[3][2] - a[1][3]*a[2][2]*a[3][1]) / det;
    res[0][1] = (-a[0][1]*a[2][2]*a[3][3] + a[0][1]*a[2][3]*a[3][2] + a[0][2]*a[2][1]*a[3][3] - a[0][2]*a[2][3]*a[3][1] - a[0][3]*a[2][1]*a[3][2] + a[0][3]*a[2][2]*a[3][1]) / det;
    res[0][2] = (a[0][1]*a[1][2]*a[3][3] - a[0][1]*a[1][3]*a[3][2] - a[0][2]*a[1][1]*a[3][3] + a[0][2]*a[1][3]*a[3][1] + a[0][3]*a[1][1]*a[3][2] - a[0][3]*a[1][2]*a[3][1]) / det;
    res[0][3] = (-a[0][1]*a[1][2]*a[2][3] + a[0][1]*a[1][3]*a[2][2] + a[0][2]*a[1][1]*a[2][3] - a[0][2]*a[1][3]*a[2][1] - a[0][3]*a[1][1]*a[2][2] + a[0][3]*a[1][2]*a[2][1]) / det;
    res[1][0] = (-a[1][0]*a[2][2]*a[3][3] + a[1][0]*a[2][3]*a[3][2] + a[1][2]*a[2][0]*a[3][3] - a[1][2]*a[2][3]*a[3][0] - a[1][3]*a[2][0]*a[3][2] + a[1][3]*a[2][2]*a[3][0]) / det;
    res[1][1] = (a[0][0]*a[2][2]*a[3][3] - a[0][0]*a[2][3]*a[3][2] - a[0][2]*a[2][0]*a[3][3] + a[0][2]*a[2][3]*a[3][0] + a[0][3]*a[2][0]*a[3][2] - a[0][3]*a[2][2]*a[3][0]) / det;
    res[1][2] = (-a[0][0]*a[1][2]*a[3][3] + a[0][0]*a[1][3]*a[3][2] + a[0][2]*a[1][0]*a[3][3] - a[0][2]*a[1][3]*a[3][0] - a[0][3]*a[1][0]*a[3][2] + a[0][3]*a[1][2]*a[3][0]) / det;
    res[1][3] = (a[0][0]*a[1][2]*a[2][3] - a[0][0]*a[1][3]*a[2][2] - a[0][2]*a[1][0]*a[2][3] + a[0][2]*a[1][3]*a[2][0] + a[0][3]*a[1][0]*a[2][2] - a[0][3]*a[1][2]*a[2][0]) / det;
    res[2][0] = (a[1][0]*a[2][1]*a[3][3] - a[1][0]*a[2][3]*a[3][1] - a[1][1]*a[2][0]*a[3][3] + a[1][1]*a[2][3]*a[3][0] + a[1][3]*a[2][0]*a[3][1] - a[1][3]*a[2][1]*a[3][0]) / det;
    res[2][1] = (-a[0][0]*a[2][1]*a[3][3] + a[0][0]*a[2][3]*a[3][1] + a[0][1]*a[2][0]*a[3][3] - a[0][1]*a[2][3]*a[3][0] - a[0][3]*a[2][0]*a[3][1] + a[0][3]*a[2][1]*a[3][0]) / det;
    res[2][2] = (a[0][0]*a[1][1]*a[3][3] - a[0][0]*a[1][3]*a[3][1] - a[0][1]*a[1][0]*a[3][3] + a[0][1]*a[1][3]*a[3][0] + a[0][3]*a[1][0]*a[3][1] - a[0][3]*a[1][1]*a[3][0]) / det;
    res[2][3] = (-a[0][0]*a[1][1]*a[2][3] + a[0][0]*a[1][3]*a[2][1] + a[0][1]*a[1][0]*a[2][3] - a[0][1]*a[1][3]*a[2][0] - a[0][3]*a[1][0]*a[2][1] + a[0][3]*a[1][1]*a[2][0]) / det;
    res[3][0] = (-a[1][0]*a[2][1]*a[3][2] + a[1][0]*a[2][2]*a[3][1] + a[1][1]*a[2][0]*a[3][2] - a[1][1]*a[2][2]*a[3][0] - a[1][2]*a[2][0]*a[3][1] + a[1][2]*a[2][1]*a[3][0]) / det;
    res[3][1] = (a[0][0]*a[2][1]*a[3][2] - a[0][0]*a[2][2]*a[3][1] - a[0][1]*a[2][0]*a[3][2] + a[0][1]*a[2][2]*a[3][0] + a[0][2]*a[2][0]*a[3][1] - a[0][2]*a[2][1]*a[3][0]) / det;
    res[3][2] = (-a[0][0]*a[1][1]*a[3][2] + a[0][0]*a[1][2]*a[3][1] + a[0][1]*a[1][0]*a[3][2] - a[0][1]*a[1][2]*a[3][0] - a[0][2]*a[1][0]*a[3][1] + a[0][2]*a[1][1]*a[3][0]) / det;
    res[3][3] = (a[0][0]*a[1][1]*a[2][2] - a[0][0]*a[1][2]*a[2][1] - a[0][1]*a[1][0]*a[2][2] + a[0][1]*a[1][2]*a[2][0] + a[0][2]*a[1][0]*a[2][1] - a[0][2]*a[1][1]*a[2][0]) / det;
}

void crossProduct3(const double a[3], const double b[3], double res[3]) {
    res[0] = a[1]*b[2] - a[2]*b[1];
    res[1] = a[2]*b[0] - a[0]*b[2];
    res[2] = a[0]*b[1] - a[1]*b[0];
}

double dotProduct3(const double a[3], const double b[3]) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}