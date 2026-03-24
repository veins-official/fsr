#ifndef OCEAN_TRAFFIC_H
#define OCEAN_TRAFFIC_H

#define OT_DEFAULT_OUTPUT "destination.png"

typedef enum {
  OT_SUCCESS             = 0,

  OT_ERR_NULL_POINTER    = 1,
  OT_ERR_INVALID_SIZE    = 2,
  OT_ERR_INVALID_PARAM   = 3,

  OT_ERR_MEMORY_ALLOC    = 100,
  OT_ERR_MEMORY_OVERFLOW = 101,

  OT_ERR_FILE_NOT_FOUND  = 200,
  OT_ERR_FILE_OPEN_READ  = 201,
  OT_ERR_FILE_OPEN_WRITE = 202,
  OT_ERR_FILE_READ       = 203,
  OT_ERR_FILE_WRITE      = 204,
  OT_ERR_FILE_EMPTY      = 205,

  OT_ERR_PNG_SIGNATURE   = 300,
  OT_ERR_PNG_CRC         = 301,
  OT_ERR_PNG_ADLER       = 302,
  OT_ERR_PNG_CHUNK       = 303,
  OT_ERR_PNG_TRUNCATED   = 304,
  OT_ERR_PNG_COLOR_TYPE  = 305,
  OT_ERR_PNG_BIT_DEPTH   = 306,
  OT_ERR_PNG_PALETTE     = 307,
  OT_ERR_PNG_DIMENSIONS  = 308,

  OT_ERR_PNG_ENCODE      = 400,
  OT_ERR_PNG_COMPRESS    = 401,
  OT_ERR_PNG_WINDOW_SIZE = 402,

  OT_ERR_INTERNAL        = 500,
  OT_ERR_UNKNOWN         = 599,
} ot_error_t;

typedef struct component_t {
  int center_x, center_y;
  int size;
} component_t;

ot_error_t ot_error_from_lodepng(unsigned int lodepng_error);

const char* ot_error_string(ot_error_t error);

ot_error_t load_png(unsigned char** dest, const char* filename, unsigned int* width, unsigned int* height);
ot_error_t write_png(const char* filename, const unsigned char* image, unsigned int width, unsigned int height);
ot_error_t write_default_png(const unsigned char* image, unsigned int width, unsigned int height);

/* Unsafe: does not check the size of dest */
ot_error_t grayscale(const unsigned char* src, unsigned char* dest, unsigned int width, unsigned int height, double red_factor, double green_factor, double blue_factor);
ot_error_t gaussian_blur(const unsigned char* src, unsigned char* dest, unsigned int width, unsigned int height, double w_center, double w_cross, double w_corner);
ot_error_t extract_edges(const unsigned char* src, unsigned char* dest, unsigned int width, unsigned int height, unsigned int threshold);
ot_error_t find_components(const unsigned char* src,
                           component_t* components,
                           unsigned int width,
                           unsigned int height,
                           unsigned int max_components_count,
                           unsigned int* components_count,
                           unsigned int min_components_size,
                           unsigned int max_components_size);

#endif

