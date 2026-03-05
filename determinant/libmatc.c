static inline int double_eq(double a, double b) {
  return fabs(a - b) <= fmax(fabs(a), fabs(b)) * DOUBLE_EPSILON;
}

typedef struct {
  double** data;
  int rows;
  int cols;
} Matrix;

