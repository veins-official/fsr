#include <stdlib.h>
#include <math.h>

#include "lodepng.h"
#include "ocean_traffic.h"

typedef struct {
  int* data;
  int head;
  int tail;
  int capacity;
} queue_t;

static queue_t* queue_create(int capacity) {
  queue_t* q = malloc(sizeof(queue_t));
  q->data = malloc(capacity * sizeof(int));
  q->head = 0;
  q->tail = 0;
  q->capacity = capacity;
  return q;
}

static void queue_free(queue_t* q) {
  free(q->data);
  free(q);
}

static void queue_push(queue_t* q, int val) {
  q->data[q->tail++] = val;
}

static int queue_pop(queue_t* q) {
  return q->data[q->head++];
}

static int queue_empty(queue_t* q) {
  return q->head >= q->tail;
}

static const int dx[] = { -1, -1, -1,  0,  0,  1,  1,  1 };
static const int dy[] = { -1,  0,  1, -1,  1, -1,  0,  1 };

static int sobel_gradient(const unsigned char* gray, int x, int y, int width, int height) {
  int gx = 0, gy = 0;
  
  if (x < 1 || x >= width - 1 || y < 1 || y >= height - 1) return 0;

  gx += -1 * gray[(y - 1) * width + x - 1];
  gx += -2 * gray[   y    * width + x - 1];
  gx += -1 * gray[(y + 1) * width + x - 1];
  gx +=  1 * gray[(y - 1) * width + x + 1];
  gx +=  2 * gray[   y    * width + x + 1];
  gx +=  1 * gray[(y + 1) * width + x + 1];

  gy += -1 * gray[(y - 1) * width + (x - 1)];
  gy += -2 * gray[(y - 1) * width +    x   ];
  gy += -1 * gray[(y - 1) * width + (x + 1)];
  gy +=  1 * gray[(y + 1) * width + (x - 1)];
  gy +=  2 * gray[(y + 1) * width +    x   ];
  gy +=  1 * gray[(y + 1) * width + (x + 1)];

  return sqrt(gx * gx + gy * gy);
}

ot_error_t ot_error_from_lodepng(unsigned int lodepng_error) {
  switch (lodepng_error) {
    case 0:  return OT_SUCCESS;

    case 78: return OT_ERR_FILE_NOT_FOUND;
    case 79: return OT_ERR_FILE_OPEN_WRITE;
    case 48: return OT_ERR_FILE_EMPTY;

    case 83: return OT_ERR_MEMORY_ALLOC;
    case 77:
    case 92:
    case 105: return OT_ERR_MEMORY_OVERFLOW;

    case 28: return OT_ERR_PNG_SIGNATURE;
    case 57: return OT_ERR_PNG_CRC;
    case 58: return OT_ERR_PNG_ADLER;
    case 30:
    case 35: return OT_ERR_PNG_TRUNCATED;
    case 31: return OT_ERR_PNG_COLOR_TYPE;
    case 32:
    case 33:
    case 34: return OT_ERR_PNG_CHUNK;
    case 37: return OT_ERR_PNG_BIT_DEPTH;
    case 38:
    case 39:
    case 103: return OT_ERR_PNG_PALETTE;
    case 93: return OT_ERR_PNG_DIMENSIONS;
    case 106: return OT_ERR_PNG_PALETTE;

    case 60:
    case 61:
    case 90: return OT_ERR_PNG_WINDOW_SIZE;
    case 84: return OT_ERR_PNG_ENCODE;
    case 68:
    case 71: return OT_ERR_PNG_ENCODE;

    default: return OT_ERR_UNKNOWN;
  }
}

const char* ot_error_string(ot_error_t error) {
  switch (error) {
    case OT_SUCCESS: return "Success";

    case OT_ERR_NULL_POINTER: return "Null pointer provided";
    case OT_ERR_INVALID_SIZE: return "Invalid image dimensions (zero or negative)";
    case OT_ERR_INVALID_PARAM: return "Invalid parameter";

    case OT_ERR_MEMORY_ALLOC: return "Memory allocation failed";
    case OT_ERR_MEMORY_OVERFLOW: return "Integer overflow in size calculation";

    case OT_ERR_FILE_NOT_FOUND: return "File not found";
    case OT_ERR_FILE_OPEN_READ: return "Cannot open file for reading";
    case OT_ERR_FILE_OPEN_WRITE: return "Cannot open file for writing";
    case OT_ERR_FILE_READ: return "Error reading file";
    case OT_ERR_FILE_WRITE: return "Error writing file";
    case OT_ERR_FILE_EMPTY: return "File is empty";

    case OT_ERR_PNG_SIGNATURE: return "Invalid PNG signature (not a PNG file)";
    case OT_ERR_PNG_CRC: return "CRC error - file structure may be corrupted";
    case OT_ERR_PNG_ADLER: return "ADLER32 error - image data is corrupted";
    case OT_ERR_PNG_CHUNK: return "Invalid PNG chunk structure";
    case OT_ERR_PNG_TRUNCATED: return "File truncated (incomplete PNG)";
    case OT_ERR_PNG_COLOR_TYPE: return "Unsupported PNG color type";
    case OT_ERR_PNG_BIT_DEPTH: return "Unsupported PNG bit depth";
    case OT_ERR_PNG_PALETTE: return "Invalid PNG palette";
    case OT_ERR_PNG_DIMENSIONS: return "PNG has zero width or height";

    case OT_ERR_PNG_ENCODE: return "PNG encoding failed";
    case OT_ERR_PNG_COMPRESS: return "PNG compression failed";
    case OT_ERR_PNG_WINDOW_SIZE: return "Invalid compression window size";

    case OT_ERR_INTERNAL: return "Internal library error";
    case OT_ERR_UNKNOWN: return "Unknown error";

    default: return "Unhandled error code";
  }
}

ot_error_t load_png(unsigned char** dest, const char* filename, unsigned int* width, unsigned int* height) {
  int error;
  if (!filename || !width || !height) return OT_ERR_NULL_POINTER;
  error = lodepng_decode32_file(dest, width, height, filename);
  return ot_error_from_lodepng(error);
}

ot_error_t write_png(const char* filename, const unsigned char* image, unsigned int width, unsigned int height) {
  unsigned char* png = NULL;
  long unsigned int pngsize;
  int error;
  
  if (!filename || !image) return OT_ERR_NULL_POINTER;
  if (width == 0 || height == 0) return OT_ERR_INVALID_SIZE;
  
  error = lodepng_encode32(&png, &pngsize, image, width, height);
  
  if (error == 0) lodepng_save_file(png, pngsize, filename);
  else return ot_error_from_lodepng(error);
  
  free(png);
  return OT_SUCCESS;
}

ot_error_t write_default_png(const unsigned char* image, unsigned int width, unsigned int height) {
  return write_png(OT_DEFAULT_OUTPUT, image, width, height);
}

ot_error_t grayscale(const unsigned char* src, unsigned char* dest, unsigned int width, unsigned int height) {
  unsigned int pngsize;
  if (!src || !dest) return OT_ERR_NULL_POINTER;

  if (width == 0 || height == 0) return OT_ERR_INVALID_SIZE;

  pngsize = width * height;

  for (unsigned int i = 0; i < pngsize; i++) {
    const unsigned char* pixel = src + i * 4;
    unsigned char gray = (unsigned char)(0.299f * pixel[0] + 0.587f * pixel[1] + 0.114f * pixel[2]);
    dest[i] = gray;
  }

  return OT_SUCCESS;
}

ot_error_t gaussian_blur(const unsigned char* src, unsigned char* dest, unsigned int width, unsigned int height) {
  const float w_center = 0.1f;
  const float w_cross = 0.1f;
  const float w_corner = 0.125f;
  
  if (!src || !dest) return OT_ERR_NULL_POINTER;
  if (width < 3 || height < 3) return OT_ERR_INVALID_SIZE;

  memcpy(dest, src, width);
  memcpy(dest + (height - 1) * width, src + (height - 1) * width, width);

  for (unsigned int i = 1; i < height - 1; i++) {
    dest[i * width] = src[i * width];
    dest[i * width + width - 1] = src[i * width + width - 1];
  }

  for (unsigned int i = 1; i < height - 1; i++) {
    for (unsigned int j = 1; j < width - 1; j++) {
      unsigned int center = i * width + j;

      double value =
      w_center * src[center] +
      w_cross  * src[(i + 1) * width + j] +
      w_cross  * src[(i - 1) * width + j] +
      w_cross  * src[i * width + (j + 1)] +
      w_cross  * src[i * width + (j - 1)] +
      w_corner * src[(i + 1) * width + (j + 1)] +
      w_corner * src[(i + 1) * width + (j - 1)] +
      w_corner * src[(i - 1) * width + (j + 1)] +
      w_corner * src[(i - 1) * width + (j - 1)];

      dest[center] = (unsigned char) value;
    }
  }

  return OT_SUCCESS;
}

ot_error_t extract_edges(const unsigned char* src, unsigned char* dest, unsigned int width, unsigned int height, unsigned int threshold) {
  if (!src || !dest) return OT_ERR_NULL_POINTER;

  for (unsigned int y = 0; y < height; y++) {
    for (unsigned int x = 0; x < width; x++) {
      unsigned int grad = sobel_gradient(src, x, y, width, height);
      dest[y * width + x] = grad > threshold ? 1 : 0;
    }
  }
  return OT_SUCCESS;
}

ot_error_t find_components(const unsigned char* src,
                           component_t* components,
                           unsigned int width,
                           unsigned int height,
                           unsigned int max_components_count,
                           unsigned int* components_count,
                           unsigned int min_components_size,
                           unsigned int max_components_size) {
  unsigned int local_components_count = 0;
  unsigned char* visited;
  queue_t* queue;
  
  if (!src || !components || !components_count) return OT_ERR_NULL_POINTER;
  
  visited = calloc(width * height, 1);
  if (!visited) return OT_ERR_MEMORY_ALLOC;
  queue = queue_create(width * height);
  if (!queue) {
    free(visited);
    return OT_ERR_MEMORY_ALLOC;
  }
  if (!queue->data) {
    free(visited);
    free(queue);
    return OT_ERR_MEMORY_ALLOC;
  }

  for (unsigned int y = 0; y < height; y++) {
    for (unsigned int x = 0; x < width; x++) {
      unsigned int index = y * width + x;

      if (src[index] && !visited[index]) {
        unsigned int size = 0;
        int sum_x = 0, sum_y = 0;

        queue->head = queue->tail = 0;
        queue_push(queue, index);
        visited[index] = 1;

        while (!queue_empty(queue)) {
          int current = queue_pop(queue);
          int cx = current % width;
          int cy = current / width;

          size++;
          sum_x += cx;
          sum_y += cy;

          for (int d = 0; d < 8; d++) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];
            if (nx >= 0 && (unsigned int) nx < width && ny >= 0 && (unsigned int) ny < height) {
              int nindex = ny * width + nx;
              if (src[nindex] && !visited[nindex]) {
                visited[nindex] = 1;
                queue_push(queue, nindex);
              }
            }
          }
        }

        if (size >= min_components_size && size <= max_components_size && local_components_count < max_components_count) {
          components[local_components_count].size = size;
          components[local_components_count].center_x = sum_x / size;
          components[local_components_count].center_y = sum_y / size;
          local_components_count++;
        }
      }
    }
  }
  
  *components_count = local_components_count;
  queue_free(queue);
  free(visited);
  return OT_SUCCESS;
}

