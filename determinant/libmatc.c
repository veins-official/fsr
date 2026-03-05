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

static MatrixError check_matrix_exists(const Matrix* matrix);
static MatrixError check_indices(const Matrix* matrix, int row, int col);
static inline int double_eq(double a, double b);

static MatrixError check_matrix_exists(const Matrix* matrix) {
  if (!matrix) return MATRIX_NULL_POINTER;
  if (!matrix->data) return MATRIX_MEMORY_ERROR;
  return MATRIX_SUCCESS;
}

static MatrixError check_indices(const Matrix* matrix, int row, int col) {
  if (row < 0 || row >= matrix->rows) return MATRIX_INDEX_ERROR;
  if (col < 0 || col >= matrix->cols) return MATRIX_INDEX_ERROR;
  return MATRIX_SUCCESS;
}

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

MatrixError matrix_get_rows(const Matrix* matrix, int* rows) {
  MatrixError status = check_matrix_exists(matrix);
  if (status != MATRIX_SUCCESS) return status;
  if (!rows) return MATRIX_NULL_POINTER;

  *rows = matrix->rows;
  return MATRIX_SUCCESS;
}

MatrixError matrix_get_cols(const Matrix* matrix, int* cols) {
  MatrixError status = check_matrix_exists(matrix);
  if (status != MATRIX_SUCCESS) return status;
  if (!cols) return MATRIX_NULL_POINTER;

  *cols = matrix->cols;
  return MATRIX_SUCCESS;
}

MatrixError matrix_is_square(const Matrix* matrix, int* result) {
  MatrixError status = check_matrix_exists(matrix);
  if (status != MATRIX_SUCCESS) return status;
  if (!result) return MATRIX_NULL_POINTER;

  *result = (matrix->rows == matrix->cols);
  return MATRIX_SUCCESS;
}

MatrixError matrix_set(Matrix* matrix, int row, int col, double value) {
  MatrixError status = check_matrix_exists(matrix);
  if (status != MATRIX_SUCCESS) return status;
  
  status = check_indices(matrix, row, col);
  if (status != MATRIX_SUCCESS) return status;
  
  matrix->data[row][col] = value;
  return MATRIX_SUCCESS;
}

MatrixError matrix_get(const Matrix* matrix, int row, int col, double* value) {
  MatrixError status = check_matrix_exists(matrix);
  if (status != MATRIX_SUCCESS) return status;
  
  status = check_indices(matrix, row, col);
  if (status != MATRIX_SUCCESS) return status;
  
  if (!value) return MATRIX_NULL_POINTER;
  
  *value = matrix->data[row][col];
  return MATRIX_SUCCESS;
}

MatrixError matrix_read(Matrix* matrix, FILE* stream) {
  MatrixError status = check_matrix_exists(matrix);
  if (status != MATRIX_SUCCESS) return status;
  
  if (!stream) return MATRIX_NULL_POINTER;
  
  for (int i = 0; i < matrix->rows; i++) {
    for (int j = 0; j < matrix->cols; j++) {
      if (fscanf(stream, "%lf", &matrix->data[i][j]) != 1) {
        return MATRIX_READ_ERROR;
      }
    }
  }
  
  return MATRIX_SUCCESS;
}

