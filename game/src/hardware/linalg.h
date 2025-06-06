
typedef double quaternion[4];

void quat_fromEulers(double *rv, double dt, quaternion res);

void quat_multiply(quaternion r, quaternion q, quaternion res);

void matmul(double *a, double *b, const unsigned int size, double *res);

void inverse4x4(double a[4][4], double res[4][4]);
