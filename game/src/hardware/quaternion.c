#include <math.h>

/*
 * Assumes that the timestep is small
 */
void fromEulers(double *rv, double dt,  quaternion res) {
    double squaredSum = 0.0;
    for (int i = 0; i < 3; i++) {
        squaredSum += rv[i] * rv[i];
    }

    double m = sqrt(squaredSum);

    res[0] = cos(m * dt / 2);
    res[1] = sin(m * dt / 2) * (rv[0] / m);
    res[2] = sin(m * dt / 2) * (rv[1] / m);
    res[3] = sin(m * dt / 2) * (rv[2] / m);
}


void multiplyQuat(

