#ifndef PACKED_PIXEL_DATA_DEF_H
#define PACKED_PIXEL_DATA_DEF_H

#include <stdint.h>

enum {
  PACKED_PIXEL_DATA_LINE_SIZE = 8U,
  PACKED_PIXEL_DATA_DISPLAY_SIZE = 64U,
};

typedef struct
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} packed_pixel_data_color;

typedef struct __attribute__((__packed__))
{
  uint8_t bytes[3U];
} packed_pixel_data_two_pixels;

#endif
