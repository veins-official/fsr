#include <stdio.h>
#include <stdlib.h>

#include "lodepng.h"
#include "ocean_traffic.h"

ot_error_t ot_error_from_lodepng(unsigned lodepng_error) {
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

ot_error_t write_png(const char* filename, const unsigned char* image, unsigned width, unsigned height) {
  unsigned char* png = NULL;
  long unsigned int pngsize;
  int error;
  
  if (!filename || !image) return OT_ERR_NULL_POINTER;
  if (width == 0 || height == 0) return OT_ERR_INVALID_SIZE;
  
  error = lodepng_encode32(&png, &pngsize, image, width, height);
  
  if(error == 0) lodepng_save_file(png, pngsize, filename);
  else return ot_error_from_lodepng(error);
  
  free(png);
  return OT_SUCCESS;
}

