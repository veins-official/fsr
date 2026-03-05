#ifndef LIBMATC_H
#define LIBMATC_H

#define DOUBLE_EPSILON 1e-9

typedef enum {
  MATRIX_SUCCESS      = 0,
  MATRIX_INVALID_SIZE = 1,
  MATRIX_MEMORY_ERROR = 2,
  MATRIX_READ_ERROR   = 3
} DistResult;

static inline int double_eq(double a, double b) {
  return fabs(a - b) <= fmax(fabs(a), fabs(b)) * DOUBLE_EPSILON;
}

#endif

