
typedef double quaternion[4];


void fromEulers(double *rv, double dt, quaternion res);

void multiplyQuat(quaternion r, quaternion q, quaternion res);
