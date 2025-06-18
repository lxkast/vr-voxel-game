
typedef double quaternion[4];

void quat_fromEulers(double *rv, double dt, quaternion res);

void quat_multiply(quaternion r, quaternion q, quaternion res);
double quat_normalise(quaternion r, quaternion res);
void quat_vecmul(quaternion q, double vector[3], double res[3]);
void mat_divs(const double *a, int rows, int cols, double factor, double *res);
void matmul(const double *a, const double *b,
            unsigned int rows_a, unsigned int cols_a, unsigned int rows_b, unsigned int cols_b,
            double *res);
void mat_transpose(const double *a, unsigned int rows_a, unsigned int cols_a, double *res);
void mat_add(const double *a, const double *b, int rows, int cols, double *res);
void vec_normalise(double *r, int n, double *res);
void mat_sub(const double *a, const double *b, int rows, int cols, double *res);

void inverse4x4(double a[4][4], double res[4][4]);
void inverse3x3(double a[3][3], double res[3][3]);
double dotProduct3(const double a[3], const double b[3]);
void crossProduct3(const double a[3], const double b[3], double res[3]);