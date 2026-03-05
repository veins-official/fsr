#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "libmatc.h"

#define DOUBLE_EPSILON 1e-9

struct Matrix {
  double** data;
  int rows;
  int cols;
};

static inline int double_is_zero(double x);
static inline int double_eq(double a, double b);
static MatrixError check_matrix_exists(const Matrix* matrix);
static MatrixError check_indices(const Matrix* matrix, int row, int col);
static MatrixError check_row_index(const Matrix* matrix, int row);
static MatrixError gauss_forward(Matrix* matrix, double* det, int* sign);
static MatrixError determinant_square(const Matrix* matrix, double* result);

static inline int double_is_zero(double x) {
  return fabs(x) <= DOUBLE_EPSILON;
}

static inline int double_eq(double a, double b) {
  return fabs(a - b) <= fmax(fabs(a), fabs(b)) * DOUBLE_EPSILON;
}

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

static MatrixError check_row_index(const Matrix* matrix, int row) {
  if (row < 0 || row >= matrix->rows) return MATRIX_INDEX_ERROR;
  return MATRIX_SUCCESS;
}

static MatrixError gauss_forward(Matrix* matrix, double* det, int* sign) {
  *det = 1.0;
  *sign = 1;

  for (int i = 0; i < matrix->rows; i++) {
    int pivot_row;
    MatrixError status = matrix_find_nonzero_in_col(matrix, i, i, &pivot_row);
    if (status != MATRIX_SUCCESS) return status;

    if (pivot_row == -1) {
      *det = 0.0;
      return MATRIX_SINGULAR;
    }

    if (pivot_row != i) {
      status = matrix_swap_rows(matrix, i, pivot_row);
      if (status != MATRIX_SUCCESS) return status;
      *sign = -(*sign);
    }

    *det *= matrix->data[i][i];

    status = matrix_eliminate_below(matrix, i, i);
    if (status != MATRIX_SUCCESS && status != MATRIX_SINGULAR) {
      return status;
    }
  }

  return MATRIX_SUCCESS;
}

static MatrixError determinant_square(const Matrix* matrix, double* result) {
  Matrix* temp = NULL;
  double det;
  int sign;
  
  MatrixError status = matrix_copy(matrix, &temp);
  if (status != MATRIX_SUCCESS) return status;
  
  status = gauss_forward(temp, &det, &sign);
  
  if (status == MATRIX_SUCCESS) {
    *result = sign * det;
  } else {
    *result = 0.0;
  }
  
  matrix_free(&temp);
  return status;
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
    (*matrix)->data[i] = (double*)malloc(cols * sizeof(double));
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

MatrixError matrix_print(const Matrix* matrix) {
  MatrixError status = check_matrix_exists(matrix);
  if (status != MATRIX_SUCCESS) return status;
  
  printf("Matrix %dx%d:\n", matrix->rows, matrix->cols);
  for (int i = 0; i < matrix->rows; i++) {
    for (int j = 0; j < matrix->cols; j++) {
      printf("%6.2f ", matrix->data[i][j]);
    }
    printf("\n");
  }
  
  return MATRIX_SUCCESS;
}

MatrixError matrix_copy(const Matrix* const source, Matrix** const destination) {
  MatrixError status = check_matrix_exists(source);
  if (status != MATRIX_SUCCESS) return status;
  
  if (!destination) return MATRIX_NULL_POINTER;
  
  status = matrix_create(destination, source->rows, source->cols);
  if (status != MATRIX_SUCCESS) return status;
  
  for (int i = 0; i < source->rows; i++) {
    memcpy((*destination)->data[i], source->data[i], source->cols * sizeof(double));
  }
  
  return MATRIX_SUCCESS;
}

MatrixError matrix_swap_rows(Matrix* matrix, int row1, int row2) {
  MatrixError status;
  double* temp;
  
  status = check_matrix_exists(matrix);
  if (status != MATRIX_SUCCESS) return status;
  
  status = check_row_index(matrix, row1);
  if (status != MATRIX_SUCCESS) return status;
  
  status = check_row_index(matrix, row2);
  if (status != MATRIX_SUCCESS) return status;
  
  if (row1 == row2) return MATRIX_SUCCESS;
  
  temp = matrix->data[row1];
  matrix->data[row1] = matrix->data[row2];
  matrix->data[row2] = temp;
  
  return MATRIX_SUCCESS;
}

MatrixError matrix_multiply_row(Matrix* matrix, int row, double factor) {
  MatrixError status = check_matrix_exists(matrix);
  if (status != MATRIX_SUCCESS) return status;
  
  status = check_row_index(matrix, row);
  if (status != MATRIX_SUCCESS) return status;
  
  if (double_eq(factor, 1.0)) return MATRIX_SUCCESS;
  
  for (int j = 0; j < matrix->cols; j++) {
    matrix->data[row][j] *= factor;
  }
  
  return MATRIX_SUCCESS;
}

MatrixError matrix_add_row(Matrix* matrix, int target_row, int source_row, double factor) {
  MatrixError status = check_matrix_exists(matrix);
  if (status != MATRIX_SUCCESS) return status;
  
  status = check_row_index(matrix, target_row);
  if (status != MATRIX_SUCCESS) return status;
  
  status = check_row_index(matrix, source_row);
  if (status != MATRIX_SUCCESS) return status;
  
  if (double_is_zero(factor)) return MATRIX_SUCCESS;
  
  for (int j = 0; j < matrix->cols; j++) {
    matrix->data[target_row][j] += matrix->data[source_row][j] * factor;
  }
  
  return MATRIX_SUCCESS;
}

MatrixError matrix_find_nonzero_in_col(const Matrix* matrix, int start_row, int col, int* found_row) {
  MatrixError status = check_matrix_exists(matrix);
  if (status != MATRIX_SUCCESS) return status;
  
  if (!found_row) return MATRIX_NULL_POINTER;
  
  status = check_indices(matrix, start_row, col);
  if (status != MATRIX_SUCCESS) return status;
  
  for (int i = start_row; i < matrix->rows; i++) {
    if (!double_is_zero(matrix->data[i][col])) {
      *found_row = i;
      return MATRIX_SUCCESS;
    }
  }
  
  *found_row = -1;
  return MATRIX_SUCCESS;
}

MatrixError matrix_normalize_row(Matrix* matrix, int row, int pivot_col) {
  MatrixError status;
  double pivot;
  
  status = check_matrix_exists(matrix);
  if (status != MATRIX_SUCCESS) return status;
  
  status = check_indices(matrix, row, pivot_col);
  if (status != MATRIX_SUCCESS) return status;
  
  pivot = matrix->data[row][pivot_col];
  if (double_is_zero(pivot)) {
    return MATRIX_SINGULAR;
  }
  
  if (double_eq(pivot, 1.0)) return MATRIX_SUCCESS;
  
  return matrix_multiply_row(matrix, row, 1.0 / pivot);
}

MatrixError matrix_eliminate_below(Matrix* matrix, int pivot_row, int pivot_col) {
  MatrixError status;
  double pivot;
  
  status = check_matrix_exists(matrix);
  if (status != MATRIX_SUCCESS) return status;
  
  status = check_indices(matrix, pivot_row, pivot_col);
  if (status != MATRIX_SUCCESS) return status;
  
  pivot = matrix->data[pivot_row][pivot_col];
  if (double_is_zero(pivot)) {
    return MATRIX_SINGULAR;
  }
  
  for (int i = pivot_row + 1; i < matrix->rows; i++) {
    if (!double_is_zero(matrix->data[i][pivot_col])) {
      double factor = -matrix->data[i][pivot_col] / pivot;
      status = matrix_add_row(matrix, i, pivot_row, factor);
      if (status != MATRIX_SUCCESS) return status;
    }
  }
  
  return MATRIX_SUCCESS;
}

MatrixError matrix_determinant(const Matrix* matrix, double* result) {
  MatrixError status = check_matrix_exists(matrix);
  if (status != MATRIX_SUCCESS) return status;
  if (!result) return MATRIX_NULL_POINTER;
  
  if (matrix->rows != matrix->cols) {
    return MATRIX_NOT_SQUARE;
  }
  
  return determinant_square(matrix, result);
}

const char* matrix_status_string(MatrixError status) {
  switch(status) {
    case MATRIX_SUCCESS:      return "Success";
    case MATRIX_NULL_POINTER: return "NULL pointer";
    case MATRIX_MEMORY_ERROR: return "Memory error";
    case MATRIX_READ_ERROR:   return "Reading error";
    case MATRIX_INDEX_ERROR:  return "Index out of range";
    case MATRIX_INVALID_SIZE: return "Wrong size";
    case MATRIX_SINGULAR:     return "Singular matrix";
    case MATRIX_NOT_SQUARE:   return "Not square";
    default:                  return "Unknown error";
  }
}

