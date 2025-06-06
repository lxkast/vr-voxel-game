#include <logging.h>
#include <math.h>
#include "linalg.h"

#include <string.h>
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

void quat_conjugate(quaternion q, quaternion res) {
    res[0] = q[0];
    res[1] = -q[1];
    res[2] = -q[2];
    res[3] = -q[3];
}

void quat_vecmul(quaternion q, double vector[3], double res[3]) {
    quaternion qp;
    quat_conjugate(q, qp);
    quaternion asq = {0, vector[0], vector[1], vector[2]};

    quaternion tmp;
    quat_multiply(q, asq, tmp);
    quaternion qres;
    quat_multiply(tmp, qp, qres);
    memcpy(res, asq + 1, sizeof(double) * 3);
}

/*
 * Note this function can be used in place.
 */
double quat_normalise(quaternion r, quaternion res) {
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
    return magnitude;
}

/*
 * Multiplies two NxN matrices
 */
void matmul(const double *a, const double *b,
            unsigned int rows_a, unsigned int cols_a, unsigned int rows_b, unsigned int cols_b,
            double *res) {
    if (res == a || res == b) {
        LOG_FATAL("Cannot multiply matrices in place");
    }

    for (unsigned int i = 0; i < rows_a; i++) {
        for (unsigned int j = 0; j < cols_b; j++) {
            double sum = 0.0;
            for (unsigned int k = 0; k < cols_a; k++) {
                sum += a[i * cols_a + k] * b[k * cols_b + j];
            }
            res[i * cols_b + j] = sum;
        }
    }
}

void mat_transpose(const double *a, unsigned int rows_a, unsigned int cols_a, double *res) {
    if (*a == *res) {
        LOG_FATAL("Cannot transpose matrix in place");
    }
    for (int i = 0; i < rows_a; i++) {
        for (unsigned int j = 0; j < cols_a; j++) {
            res[j * rows_a + i] = a[i * cols_a + j];
        }
    }
}

/*
 * Note can be done in place
 */
void mat_add(const double *a, const double *b, int rows, int cols, double *res) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            res[i*cols + j] = a[i*cols + j] + b[i*cols + j];
        }
    }
}

void mat_sub(const double *a, const double *b, int rows, int cols, double *res) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            res[i*cols + j] = a[i*cols + j] - b[i*cols + j];
        }
    }
}

/*
 *  Note this function can be called inline
 */
void mat_divs(const double *a, int rows, int cols, double factor, double *res) {
    for (int i = 0; i < rows * cols; i++) {
        res[i] = a[i] / factor;
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

void inverse3x3(double a[3][3], double res[3][3]) {
    double det = a[0][0]*a[1][1]*a[2][2] - a[0][0]*a[1][2]*a[2][1] - a[0][1]*a[1][0]*a[2][2] + a[0][1]*a[1][2]*a[2][0] + a[0][2]*a[1][0]*a[2][1] - a[0][2]*a[1][1]*a[2][0];
    res[0][0] = (a[1][1]*a[2][2] - a[1][2]*a[2][1]) / det;
    res[0][1] = (-a[0][1]*a[2][2] + a[0][2]*a[2][1]) / det;
    res[0][2] = (a[0][1]*a[1][2] - a[0][2]*a[1][1]) / det;
    res[1][0] = (-a[1][0]*a[2][2] + a[1][2]*a[2][0]) / det;
    res[1][1] = (a[0][0]*a[2][2] - a[0][2]*a[2][0]) / det;
    res[1][2] = (-a[0][0]*a[1][2] + a[0][2]*a[1][0]) / det;
    res[2][0] = (a[1][0]*a[2][1] - a[1][1]*a[2][0]) / det;
    res[2][1] = (-a[0][0]*a[2][1] + a[0][1]*a[2][0]) / det;
    res[2][2] = (a[0][0]*a[1][1] - a[0][1]*a[1][0]) / det;
}

void crossProduct3(const double a[3], const double b[3], double res[3]) {
    res[0] = a[1]*b[2] - a[2]*b[1];
    res[1] = a[2]*b[0] - a[0]*b[2];
    res[2] = a[0]*b[1] - a[1]*b[0];
}

double dotProduct3(const double a[3], const double b[3]) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}