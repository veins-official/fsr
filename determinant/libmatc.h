#ifndef LIBMATC_H
#define LIBMATC_H

#include <stdio.h>

typedef enum {
  MATRIX_SUCCESS      = 0,
  MATRIX_NULL_POINTER = 1,
  MATRIX_MEMORY_ERROR = 2,
  MATRIX_READ_ERROR   = 3,
  MATRIX_INDEX_ERROR  = 4,
  MATRIX_INVALID_SIZE = 5,
  MATRIX_SINGULAR     = 6,
  MATRIX_NOT_SQUARE   = 7,
  
} MatrixError;

typedef struct Matrix Matrix;

MatrixError matrix_create(Matrix** matrix, int rows, int cols);
MatrixError matrix_free(Matrix** matrix);
MatrixError matrix_copy(const Matrix* source, Matrix** destination);

MatrixError matrix_get_rows(const Matrix* matrix, int* rows);
MatrixError matrix_get_cols(const Matrix* matrix, int* cols);
MatrixError matrix_is_square(const Matrix* matrix, int* result);
MatrixError matrix_set(Matrix* matrix, int row, int col, double value);
MatrixError matrix_get(const Matrix* matrix, int row, int col, double* value);

MatrixError matrix_read(Matrix* matrix, FILE* stream);
MatrixError matrix_print(const Matrix* matrix);

MatrixError matrix_swap_rows(Matrix* matrix, int row1, int row2);
MatrixError matrix_multiply_row(Matrix* matrix, int row, double factor);
MatrixError matrix_add_row(Matrix* matrix, int target_row, int source_row, double factor);
MatrixError matrix_find_nonzero_in_col(const Matrix* matrix, int start_row, int col, int* found_row);
MatrixError matrix_normalize_row(Matrix* matrix, int row, int pivot_col);
MatrixError matrix_eliminate_below(Matrix* matrix, int pivot_row, int pivot_col);

MatrixError matrix_determinant(const Matrix* matrix, double* result);

const char* matrix_status_string(MatrixError status);

#endif

