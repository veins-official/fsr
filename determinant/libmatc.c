#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "libmatc.h"

#define DOUBLE_EPSILON 1e-9

typedef struct {
  double** data;
  int rows;
  int cols;
} Matrix;

static inline int double_eq(double a, double b);

static inline int double_eq(double a, double b) {
  return fabs(a - b) <= fmax(fabs(a), fabs(b)) * DOUBLE_EPSILON;
}


