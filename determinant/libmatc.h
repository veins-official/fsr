#ifndef LIBMATC_H
#define LIBMATC_H

#define DOUBLE_EPSILON 1e-9

typedef enum {
  MATRIX_SUCCESS      = 0,
  MATRIX_NULL_POINTER = 1,
  MATRIX_MEMORY_ERROR = 2,
  MATRIX_READ_ERROR   = 3,
  MATRIX_INDEX_ERROR  = 4,
  MATRIX_INVALID_SIZE = 5,
  
} MatrixError;

typedef struct Matrix Matrix;

MatrixError matrix_create(Matrix** const matrix, const int rows, const int cols);
MatrixError matrix_free(Matrix** const matrix);

MatrixError matrix_get_rows(const Matrix* const matrix, int* const rows);
MatrixError matrix_get_cols(const Matrix* const matrix, int* const cols);
MatrixError matrix_is_square(const Matrix* const matrix, int* const result);

MatrixError matrix_set(Matrix* const matrix, const int row, const int col, const double value);
MatrixError matrix_get(const Matrix* const matrix, const int row, const int col, double* const value);

MatrixError matrix_read(Matrix* const matrix, FILE* const stream);
MatrixError matrix_print(const Matrix* const matrix);

MatrixError matrix_determinant(const Matrix* const matrix, double* const result);

MatrixError matrix_copy(const Matrix* const source, Matrix** const destination);

const char* det_status_string(const MatrixError error);

#endif

