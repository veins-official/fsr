#include <stdio.h>
#include <stdlib.h>

#include "ocean_traffic.h"

int main(int argc, char* argv[]) {
  unsigned char* grayscale_picture;
  unsigned char* picture;
  unsigned width, height;
  ot_error_t error;
  
  if (argc < 2) {
    printf("Usage: %s input_file [output_file]\n", argv[0]);
    return 1;
  }

  
  error = load_png(&picture, argv[1], &width, &height);
  if (error != OT_SUCCESS) {
    printf("%s\n", ot_error_string(error));
    return 1;
  }

  grayscale_picture = (unsigned char*)malloc(width * height * sizeof(unsigned char));
  if (!grayscale_picture) {
    printf("Memory allocation failed\n");
    return 1;
  }
  
  error = grayscale(picture, grayscale_picture, width, height);
  if (error != OT_SUCCESS) {
    printf("%s\n", ot_error_string(error));
    return 1;
  }
  
  for (unsigned int i = 0; i < height; i++) {
    for (unsigned int j = 0; j < width; j++) {
      unsigned int color = grayscale_picture[i * width + j];
      unsigned int index = (i * width + j) * 4;
      picture[index    ] = color;
      picture[index + 1] = color;
      picture[index + 2] = color;
      picture[index + 3] = color;
    }
  }

  if (argc == 3) error = write_png(argv[2], picture, width, height);
  else error = write_default_png(picture, width, height);
  if (error != OT_SUCCESS) {
    printf("%s\n", ot_error_string(error));
    return 1;
  }

  free(grayscale_picture);
  free(picture);

  return 0;
}

