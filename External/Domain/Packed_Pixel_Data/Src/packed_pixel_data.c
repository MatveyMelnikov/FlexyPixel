#include "packed_pixel_data.h"
#include <stdbool.h>

// Defines -------------------------------------------------------------------

#define GET_NUM_FROM_CHAR(ch) \
  ((ch) - (uint8_t)('0'))
#define GET_HALF_BYTE(bt) \
  ((bt) & 0x0f)

// Static variables ----------------------------------------------------------

// Static functions ----------------------------------------------------------

static void packed_pixel_data_pack_line(
  const packed_pixel_data_color *const input,
  uint8_t *const output,
  uint8_t line_index,
  uint8_t display_index
);
__attribute__((always_inline))
inline static void packed_pixel_data_pack_two_pixels(
  packed_pixel_data_two_pixels *output_pixels,
  const packed_pixel_data_color *input_pixels
);
__attribute__((always_inline))
inline static void packed_pixel_data_reverse_pack_two_pixels(
  packed_pixel_data_two_pixels *output_pixels,
  const packed_pixel_data_color *input_pixels
);
static void packed_pixel_data_unpack_line(
  const uint8_t *const input,
  packed_pixel_data_color *const output,
  uint8_t line_index,
  uint8_t display_index
);
__attribute__((always_inline))
inline static void packed_pixel_data_reverse_unpack_two_pixels(
  packed_pixel_data_color *output_pixels,
  const packed_pixel_data_two_pixels *input_pixels
);
__attribute__((always_inline))
inline static void packed_pixel_data_unpack_two_pixels(
  packed_pixel_data_color *output_pixels,
  const packed_pixel_data_two_pixels *input_pixels
);
__attribute__((always_inline))
inline static uint16_t packed_pixel_data_get_packed_index(
  uint8_t pixel_index,
  bool is_even_line,
  uint8_t line_offset,
  uint8_t display_offset
);
// Implementations -----------------------------------------------------------

void packed_pixel_data_pack(
  const packed_pixel_data_color *const input,
  uint8_t *const output,
  uint8_t displays_amount
)
{
  for (
    uint8_t display_index = 0;
    display_index < displays_amount;
    display_index++
  )
  {
    for (
      uint8_t line_index = 0U;
      line_index < PACKED_PIXEL_DATA_LINE_SIZE;
      line_index++
    )
    {
      packed_pixel_data_pack_line(input, output, line_index, display_index);
    }
  }
}

static void packed_pixel_data_pack_line(
  const packed_pixel_data_color *const input,
  uint8_t *const output,
  uint8_t line_index,
  uint8_t display_index
)
{
  // input:  |rrrr.rrrr|gggg.gggg|bbbb.bbbb|rrrr...
  // output: |rrrr.gggg|bbbb.rrrr|gggg.bbbb|rrrr...

  uint8_t line_offset = line_index * PACKED_PIXEL_DATA_LINE_SIZE;
  uint8_t display_offset = display_index * PACKED_PIXEL_DATA_DISPLAY_SIZE;
  bool is_even_line = (line_index % 2) == 0;

  for (
    uint8_t pixel_index = 0U;
    pixel_index < PACKED_PIXEL_DATA_LINE_SIZE;
    pixel_index += 2U
  )
  {
    uint16_t packed_pixel_index = packed_pixel_data_get_packed_index(
      pixel_index,
      is_even_line,
      display_offset,
      line_offset
    );
    uint16_t input_pixel_offset = pixel_index + line_offset + display_offset;
    
    const packed_pixel_data_color *input_pixels = 
      (const packed_pixel_data_color*)input + input_pixel_offset;
    packed_pixel_data_two_pixels *output_pixels = 
      (packed_pixel_data_two_pixels*)(output) + packed_pixel_index;

    if (is_even_line)
      packed_pixel_data_reverse_pack_two_pixels(output_pixels, input_pixels);
    else
      packed_pixel_data_pack_two_pixels(output_pixels, input_pixels);
  }
}

__attribute__((always_inline))
inline static void packed_pixel_data_pack_two_pixels(
  packed_pixel_data_two_pixels *output_pixels,
  const packed_pixel_data_color *input_pixels
)
{
  output_pixels->bytes[0] = (GET_HALF_BYTE(input_pixels->red) << 4U) |
    GET_HALF_BYTE(input_pixels->green);
  output_pixels->bytes[1] = (GET_HALF_BYTE(input_pixels->blue) << 4U) |
    GET_HALF_BYTE((input_pixels + 1)->red);
  output_pixels->bytes[2] = 
    (GET_HALF_BYTE((input_pixels + 1)->green) << 4U) |
      GET_HALF_BYTE((input_pixels + 1)->blue);
}

__attribute__((always_inline))
inline static void packed_pixel_data_reverse_pack_two_pixels(
  packed_pixel_data_two_pixels *output_pixels,
  const packed_pixel_data_color *input_pixels
)
{
  output_pixels->bytes[0] = (GET_HALF_BYTE((input_pixels + 1)->red) << 4U) |
    GET_HALF_BYTE((input_pixels + 1)->green);
  output_pixels->bytes[1] = (GET_HALF_BYTE((input_pixels + 1)->blue) << 4U) |
    GET_HALF_BYTE(input_pixels->red);
  output_pixels->bytes[2] = 
    (GET_HALF_BYTE(input_pixels->green) << 4U) |
      GET_HALF_BYTE(input_pixels->blue);
}

void packed_pixel_data_unpack(
  const uint8_t *const input,
  packed_pixel_data_color *const output,
  uint8_t displays_amount
)
{
  for (
    uint8_t display_index = 0;
    display_index < displays_amount;
    display_index++
  )
  {
    for (
      uint8_t line_index = 0U;
      line_index < PACKED_PIXEL_DATA_LINE_SIZE;
      line_index++
    )
    {
      packed_pixel_data_unpack_line(input, output, line_index, display_index);
    }
  }
}

static void packed_pixel_data_unpack_line(
  const uint8_t *const input,
  packed_pixel_data_color *const output,
  uint8_t line_index,
  uint8_t display_index
)
{
  // input:  |rrrr.gggg|bbbb.rrrr|gggg.bbbb|rrrr...
  // output: |rrrr.rrrr|gggg.gggg|bbbb.bbbb|rrrr...
  uint8_t line_offset = line_index * PACKED_PIXEL_DATA_LINE_SIZE;
  uint8_t display_offset = display_index * PACKED_PIXEL_DATA_DISPLAY_SIZE;
  bool is_even_line = (line_index % 2) == 0;

  for (
    uint8_t pixel_index = 0U;
    pixel_index < PACKED_PIXEL_DATA_LINE_SIZE;
    pixel_index += 2U
  )
  {
    uint16_t packed_pixel_index = packed_pixel_data_get_packed_index(
      pixel_index,
      is_even_line,
      display_offset,
      line_offset
    );
    uint16_t input_pixel_offset = pixel_index + line_offset + display_offset;

    packed_pixel_data_color *output_pixels = output + input_pixel_offset;
    const packed_pixel_data_two_pixels *input_pixels = 
      (packed_pixel_data_two_pixels*)(input) + packed_pixel_index;

    if (is_even_line)
      packed_pixel_data_reverse_unpack_two_pixels(output_pixels, input_pixels);
    else
      packed_pixel_data_unpack_two_pixels(output_pixels, input_pixels);
  }
}

__attribute__((always_inline))
inline static void packed_pixel_data_unpack_two_pixels(
  packed_pixel_data_color *output_pixels,
  const packed_pixel_data_two_pixels *input_pixels
)
{
  *output_pixels = (packed_pixel_data_color) {
    .red = GET_HALF_BYTE(input_pixels->bytes[0] >> 4),
    .green = GET_HALF_BYTE(input_pixels->bytes[0]),
    .blue = GET_HALF_BYTE(input_pixels->bytes[1] >> 4)
  };
  *(output_pixels + 1) = (packed_pixel_data_color) {
    .red = GET_HALF_BYTE(input_pixels->bytes[1]),
    .green = GET_HALF_BYTE(input_pixels->bytes[2] >> 4),
    .blue = GET_HALF_BYTE(input_pixels->bytes[2])
  };
}

__attribute__((always_inline))
inline static void packed_pixel_data_reverse_unpack_two_pixels(
  packed_pixel_data_color *output_pixels,
  const packed_pixel_data_two_pixels *input_pixels
)
{
  *output_pixels = (packed_pixel_data_color) {
    .red = GET_HALF_BYTE(input_pixels->bytes[1]),
    .green = GET_HALF_BYTE(input_pixels->bytes[2] >> 4),
    .blue = GET_HALF_BYTE(input_pixels->bytes[2])
  };
  *(output_pixels + 1) = (packed_pixel_data_color) {
    .red = GET_HALF_BYTE(input_pixels->bytes[0] >> 4),
    .green = GET_HALF_BYTE(input_pixels->bytes[0]),
    .blue = GET_HALF_BYTE(input_pixels->bytes[1] >> 4)
  };
}

__attribute__((always_inline))
inline static uint16_t packed_pixel_data_get_packed_index(
  uint8_t pixel_index,
  bool is_even_line,
  uint8_t line_offset,
  uint8_t display_offset
)
{
  // Each even line starts from the end
  uint16_t packed_index = is_even_line ? 
    (3U - pixel_index / 2U) : pixel_index / 2U;
  packed_index += (display_offset / 2U) + (line_offset / 2U);

  return packed_index;
}
