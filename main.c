#include "libmatc.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  Matrix* matrix = NULL;
  MatrixError status;
  int rows, cols;
  double det;
  
  if (argc != 2 && sscanf (argv[1], "%d", &rows) == 1 && sscanf (argv[2], "%d", &cols) == 1) {
    printf ("Usage: %s rows cols\n", argv[0]);
    return 1;
  }
  
  if (rows <= 0 || cols <= 0) {
    printf("Error: Dimensions must be positive numbers\n");
    return 1;
  }
  
  status = matrix_create(&matrix, rows, cols);
  if (status != MATRIX_SUCCESS) {
    printf("Error creating matrix: %s\n", matrix_status_string(status));
    return 1;
  }

  status = matrix_read(matrix, stdin);
  if (status != MATRIX_SUCCESS) {
    printf("Matrix read error: %s\n", matrix_status_string(status));
    matrix_free(&matrix);
    return 1;
  }
  
  status = matrix_determinant(matrix, &det);
  
  if (status == MATRIX_SUCCESS) printf("Determinant = %f\n", det);
  else if (status == MATRIX_SINGULAR) printf("Determinant = 0\n");
  else printf("Error: %s\n", matrix_status_string(status));
  
  matrix_free(&matrix);

  return 0;
}
