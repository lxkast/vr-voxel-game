#ifndef ORIENTATION_H
#define ORIENTATION_H

#include "linalg.h"

extern void startOrientationThread(void);
extern void imu_getOrientation(quaternion res);

#endif