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

MatrixError matrix_create(Matrix** matrix, int rows, int cols) {
  if (!matrix) return MATRIX_NULL_POINTER;
  if (rows <= 0 || cols <= 0) return MATRIX_INVALID_SIZE;

  *matrix = (Matrix*)malloc(sizeof(Matrix));
  if (!(*matrix)) return MATRIX_MEMORY_ERROR;

  (*matrix)->rows = rows;
  (*matrix)->cols = cols;
  (*matrix)->data = (double**)malloc(rows * sizeof(double*));

  if (!(*matrix)->data) {
    free(*matrix);
    *matrix = NULL;
    return MATRIX_MEMORY_ERROR;
  }

  for (int i = 0; i < rows; i++) {
    (*matrix)->data[i] = (double*)malloc(cols, sizeof(double));
    if (!(*matrix)->data[i]) {
      for (int j = 0; j < i; j++) {
        free((*matrix)->data[j]);
      }
      free((*matrix)->data);
      free(*matrix);
      *matrix = NULL;
      return MATRIX_MEMORY_ERROR;
    }
  }

  return MATRIX_SUCCESS;
}

MatrixError matrix_free(Matrix** matrix) {
  if (!matrix || !(*matrix)) return MATRIX_SUCCESS;

  if ((*matrix)->data) {
    for (int i = 0; i < (*matrix)->rows; i++) {
      free((*matrix)->data[i]);
    }
    free((*matrix)->data);
  }

  free(*matrix);
  *matrix = NULL;

  return MATRIX_SUCCESS;
}

