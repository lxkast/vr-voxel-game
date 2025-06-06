
typedef double quaternion[4];

void quat_fromEulers(double *rv, double dt, quaternion res);

void quat_multiply(quaternion r, quaternion q, quaternion res);
void quat_normalise(quaternion r, quaternion res);

void matmul(double *a, double *b, const unsigned int size, double *res);
void vec_normalise(double *r, int n, double *res);

void inverse4x4(double a[4][4], double res[4][4]);
double dotProduct3(const double a[3], const double b[3]);
void crossProduct3(const double a[3], const double b[3], double res[3]);