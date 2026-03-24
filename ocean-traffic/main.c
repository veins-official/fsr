#include <stdio.h>
#include <stdlib.h>

#include "ocean_traffic.h"

int main(int argc, char* argv[]) {
  unsigned char* grayscale_picture;
  component_t components[5000];
  unsigned char* blur_picture;
  unsigned char* edge_picture;
  unsigned char* picture;
  unsigned int width, height;
  unsigned int comp_count;
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
    free(picture);
    printf("Memory allocation failed\n");
    return 1;
  }
  
  blur_picture = (unsigned char*)malloc(width * height * sizeof(unsigned char));
  if (!blur_picture) {
    free(grayscale_picture);
    free(picture);
    printf("Memory allocation failed\n");
    return 1;
  }
  
  edge_picture = (unsigned char*)malloc(width * height * sizeof(unsigned char));
  if (!edge_picture) {
    free(grayscale_picture);
    free(blur_picture);
    free(picture);
    printf("Memory allocation failed\n");
    return 1;
  }
  
  error = grayscale(picture, grayscale_picture, width, height, 0.299, 0.587, 0.114);
  if (error != OT_SUCCESS) {
    free(grayscale_picture);
    free(blur_picture);
    free(edge_picture);
    free(picture);
    printf("%s\n", ot_error_string(error));
    return 1;
  }
  
  error = extract_edges(grayscale_picture, edge_picture, width, height, 50);
  if (error != OT_SUCCESS) {
    free(grayscale_picture);
    free(blur_picture);
    free(edge_picture);
    free(picture);
    printf("%s\n", ot_error_string(error));
    return 1;
  }
  
  error = find_components(edge_picture, components, width, height, 5000, &comp_count, 2, 100);
  if (error != OT_SUCCESS) {
    free(grayscale_picture);
    free(blur_picture);
    free(edge_picture);
    free(picture);
    printf("%s\n", ot_error_string(error));
    return 1;
  }
  
  error = gaussian_blur(grayscale_picture, blur_picture, width, height, 0.1, 0.1, 0.125);
  if (error != OT_SUCCESS) {
    free(grayscale_picture);
    free(blur_picture);
    free(edge_picture);
    free(picture);
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
      picture[index + 3] = 255;
    }
  }
  
  for (unsigned int i = 0; i < comp_count; i++) {
    component_t c = components[i];

    int cx = c.center_x;
    int cy = c.center_y;

    for (int dy = -2; dy <= 2; dy++) {
      for (int dx = -2; dx <= 2; dx++) {
        if (dx == 0 || dy == 0) {
          int x = cx + dx;
          int y = cy + dy;
          if (x >= 0 && (unsigned int) x < width && y >= 0 && (unsigned int) y < height) {
            int index = y * width * 4 + x * 4;
            picture[  index  ] = 0;
            picture[index + 1] = 255;
            picture[index + 2] = 0;
            picture[index + 3] = 255;
          }
        }
      }
    }
  }

  if (argc == 3) error = write_png(argv[2], picture, width, height);
  else error = write_default_png(picture, width, height);
  if (error != OT_SUCCESS) {
    printf("%s\n", ot_error_string(error));
    free(grayscale_picture);
    free(blur_picture);
    free(edge_picture);
    free(picture);
    return 1;
  }

  free(grayscale_picture);
  free(blur_picture);
  free(edge_picture);
  free(picture);

  return 0;
}

