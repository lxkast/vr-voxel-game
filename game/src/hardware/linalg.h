
typedef double quaternion[4];

void quat_fromEulers(double *rv, double dt, quaternion res);

void quat_multiply(quaternion r, quaternion q, quaternion res);

void matmul(double *a, double *b, int size, double *res);
