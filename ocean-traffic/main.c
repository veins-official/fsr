#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ocean_traffic.h"

typedef struct {
  char* input_file;
  char* output_file;
  unsigned int max_components;
  unsigned int min_component_size;
  unsigned int max_component_size;
  unsigned int edge_threshold;
  int save_grayscale;
  int save_edges;
} program_options_t;

static int parse_options(int argc, char* argv[], program_options_t* options);
static void print_usage(const char* program_name);
static void print_version(void);

static void print_usage(const char* program_name) {
  printf("Usage: %s [OPTIONS]\n", program_name);
  printf("Options:\n");
  printf("  -i FILE         Input PNG file (required)\n");
  printf("  -o FILE         Output PNG file (default: output.png)\n");
  printf("  -m N            Maximum number of components (default: 500)\n");
  printf("  -f N            Minimum component size (default: 1)\n");
  printf("  -l N            Maximum component size (default: 100)\n");
  printf("  -t N            Edge detection threshold (default: 125)\n");
  printf("  -a              Save grayscale image (grayscale.png)\n");
  printf("  -e              Save edges image (edges.png)\n");
  printf("  -v              Display version information\n");
  printf("  -h              Show this help message\n");
}

static void print_version(void) {
  printf("Ocean traffic 1.0\n");
  printf("Target: x86_64-redhat-linux\n");
  printf("Copyright (C) 2026 Student of The Faculty of Space Research at Moscow State University\n");
  printf("License: MIT\n");
  printf("Author: Kholodov Ilya Alekseevich <seal0x1c3@gmail.com>\n");
}

static int parse_options(int argc, char* argv[], program_options_t* options) {
  int i;

  options->input_file = NULL;
  options->output_file = (char*) "output.png";
  options->max_components = 500;
  options->min_component_size = 1;
  options->max_component_size = 100;
  options->edge_threshold = 125;
  options->save_grayscale = 0;
  options->save_edges = 0;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) {
      if (i + 1 < argc) options->input_file = argv[++i];
    }
    else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
      if (i + 1 < argc) options->output_file = argv[++i];
    }
    else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--max-components") == 0) {
      if (i + 1 < argc) options->max_components = atoi(argv[++i]);
    }
    else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--min-size") == 0) {
      if (i + 1 < argc) options->min_component_size = atoi(argv[++i]);
    }
    else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--max-size") == 0) {
      if (i + 1 < argc) options->max_component_size = atoi(argv[++i]);
    }
    else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--threshold") == 0) {
      if (i + 1 < argc) options->edge_threshold = atoi(argv[++i]);
    }
    else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--save-grayscale") == 0) {
      options->save_grayscale = 1;
    }
    else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--save-edges") == 0) {
      options->save_edges = 1;
    }
    else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
      print_version();
      return 0;
    }
    else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      print_usage(argv[0]);
      return 0;
    }
  }

  if (!options->input_file) {
    printf("Error: Input file is required\n");
    print_usage(argv[0]);
    return 0;
  }

  return 1;
}

int main(int argc, char* argv[]) {
  program_options_t options;
  unsigned char* grayscale_picture = NULL;
  unsigned char* edge_picture = NULL;
  unsigned char* picture = NULL;
  component_t* components = NULL;
  unsigned int components_count;
  unsigned int width, height;
  unsigned int areas_count = 9;
  ot_error_t error;
  unsigned int i;
  
  unsigned int areas[][4] = {
    {532,  24,  727, 287},
    {577, 288, 1124, 637},
    {526, 486,  584, 531},
    {536, 531,  583, 563},
    {566, 576,  582, 598},
    {510, 337,  585, 412},
    {509, 293,  564, 315},
    {550, 312,  564, 330},
    {505, 328,  550, 336}
  };


  if (!parse_options(argc, argv, &options)) {
    return 1;
  }
  
  components = (component_t*)malloc(options.max_components * sizeof(component_t));

  error = load_png(&picture, &width, &height, options.input_file);
  if (error != OT_SUCCESS) {
    printf("Error loading image: %s\n", ot_error_string(error));
    return 1;
  }

  grayscale_picture = (unsigned char*)malloc(width * height * sizeof(unsigned char));
  if (!grayscale_picture) {
    printf("Memory allocation failed\n");
    free(picture);
    return 1;
  }
  
  edge_picture = (unsigned char*)malloc(width * height * sizeof(unsigned char));
  if (!edge_picture) {
    printf("Memory allocation failed\n");
    free(grayscale_picture);
    free(picture);
    return 1;
  }

  error = grayscale(grayscale_picture, picture, width, height, 0.299, 0.587, 0.114);
  if (error != OT_SUCCESS) {
    printf("Error in grayscale conversion: %s\n", ot_error_string(error));
    free(grayscale_picture);
    free(edge_picture);
    free(picture);
    return 1;
  }

  if (options.save_grayscale) {
    unsigned char* log_picture = (unsigned char*)malloc(4 * width * height * sizeof(unsigned char));
    if (!log_picture) {
      printf("Error saving grayscale image: Memory allocation failed\n");
    } else {
      error = bw_to_rgba(log_picture, grayscale_picture, width, height);
      if (error != OT_SUCCESS) {
        printf("Error saving grayscale image: %s\n", ot_error_string(error));
      } else {
        error = write_png(log_picture, width, height, "grayscale.png");
        if (error != OT_SUCCESS) {
          printf("Error saving grayscale image: %s\n", ot_error_string(error));
        } else {
          printf("Grayscale image saved as grayscale.png\n");
        }
      }
    }
    free(log_picture);
  }

  error = extract_edges(edge_picture, grayscale_picture, width, height, options.edge_threshold);
  if (error != OT_SUCCESS) {
    printf("Error in edge detection: %s\n", ot_error_string(error));
    free(grayscale_picture);
    free(edge_picture);
    free(picture);
    return 1;
  }
  
  if (options.save_edges) {
    unsigned char* log_picture = (unsigned char*)malloc(4 * width * height * sizeof(unsigned char));
    if (!log_picture) {
      printf("Error saving grayscale image: Memory allocation failed\n");
    } else {
      error = bw_to_rgba(log_picture, edge_picture, width, height);
      if (error != OT_SUCCESS) {
        printf("Error saving edges image: %s\n", ot_error_string(error));
      } else {
        error = write_png(log_picture, width, height, "edges.png");
        if (error != OT_SUCCESS) {
          printf("Error saving edges image: %s\n", ot_error_string(error));
        } else {
          printf("Edges image saved as edges.png\n");
        }
      }
    }
    free(log_picture);
  }

  error = find_components(components, &components_count, areas, edge_picture, width, height, options.max_components, options.min_component_size, options.max_component_size, areas_count);
  if (error != OT_SUCCESS) {
    printf("Error finding components: %s\n", ot_error_string(error));
    free(grayscale_picture);
    free(edge_picture);
    free(picture);
    return 1;
  }

  printf("Found %u components\n", components_count);
  
  for (i = 0; i < components_count; i++) {
  component_t c = components[i];
  int cx = c.center_x;
  int cy = c.center_y;
  int dy, dx;

    for (dy = -2; dy <= 2; dy++) {
      for (dx = -2; dx <= 2; dx++) {
        if (dx == 0 || dy == 0) {
          int x = cx + dx;
          int y = cy + dy;
          if (x >= 0 && (unsigned int)x < width && y >= 0 && (unsigned int)y < height) {
            int index = y * width * 4 + x * 4;
            picture[index] = 0;
            picture[index + 1] = 255;
            picture[index + 2] = 0;
            picture[index + 3] = 255;
          }
        }
      }
    }
  }

  error = write_png(picture, width, height, options.output_file);
  if (error != OT_SUCCESS) {
    printf("Error saving output: %s\n", ot_error_string(error));
    free(grayscale_picture);
    free(edge_picture);
    free(picture);
    return 1;
  }

  printf("Output saved as %s\n", options.output_file);
  return (error == OT_SUCCESS) ? 0 : 1;
}

