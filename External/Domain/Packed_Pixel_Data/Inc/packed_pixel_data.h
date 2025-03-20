#ifndef PACKED_PIXEL_DATA_H
#define PACKED_PIXEL_DATA_H

#include "packed_pixel_data_def.h"

// input and output - 864 pixels
void packed_pixel_data_pack(
  const packed_pixel_data_color *const input,
  uint8_t *const output,
  uint8_t displays_amount
);
// input and output - 864 pixels
void packed_pixel_data_unpack(
  const uint8_t *const input,
  packed_pixel_data_color *const output,
  uint8_t displays_amount
);

#endif
