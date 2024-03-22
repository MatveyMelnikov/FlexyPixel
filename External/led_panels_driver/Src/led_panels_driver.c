#include "led_panels_driver.h"
#include "led_panels_defs.h"
#include "led_panels_io.h"
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include <math.h>

// Defines -------------------------------------------------------------------

typedef struct
{
  uint16_t byte_offset;
  uint8_t is_first;
} led_panels_pixel_pos;


#define PWM_PIXEL_SIZE 24
#define GET_PANEL_SIZE(index, sizes) \
  (uint16_t)(sizes[index])

// Programming manual stm32f10xxx (pg. 28)
// bit_word_offset = (byte_offset x 32) + (bit_number x 4)
// bit_word_addr = bit_band_base + bit_word_offset
#define GET_BIT(addr, bit_num) \
  (volatile uint32_t*)( \
    0x22000000 + (((uint32_t)(addr) & 0xfffff) << 5) + ((bit_num) << 2) \
  )

// Static variables ----------------------------------------------------------

// Static functions ----------------------------------------------------------

__attribute__((always_inline))
inline static void convert_color_channel_to_pwm_data(
  uint8_t *const pwm_data,
  volatile uint32_t * color_channel // first bit of channel (4 bits, bit banding)
)
{
  for (uint8_t i = 0; i < 4; i++)
  {
    *(pwm_data + 4 + i) = (bool)(*(color_channel - i)) ? 
      LED_PANELS_1_VALUE : LED_PANELS_0_VALUE;
  }
}

__attribute__((always_inline))
inline static void set_pixels_to_pwm_data(
  uint8_t *pwm_data,
  uint8_t *const pixel_data,
  uint16_t *index
)
{
  uint8_t *current_pixels = pixel_data + *index;

  // first pixel
  convert_color_channel_to_pwm_data(
    pwm_data,
    GET_BIT(current_pixels, 3)
  ); // g
  convert_color_channel_to_pwm_data(
    pwm_data + 8,
    GET_BIT(current_pixels, 7)
  ); // r
  convert_color_channel_to_pwm_data(
    pwm_data + 16,
    GET_BIT(current_pixels + 1, 7)
  ); // b

  // second pixel
  convert_color_channel_to_pwm_data(
    pwm_data + 24,
    GET_BIT(current_pixels + 2, 7)
  ); // g
  convert_color_channel_to_pwm_data(
    pwm_data + 32,
    GET_BIT(current_pixels + 1, 3)
  ); // r
  convert_color_channel_to_pwm_data(
    pwm_data + 40,
   GET_BIT(current_pixels + 2, 3)
  ); // b

  *index += 3;
}

static void get_pixel_from_data(
  uint8_t *const pixel_data,
  led_panels_pixel_pos *const pixel_pos,
  led_panels_color * color
)
{
  uint8_t *current_pixel_data = pixel_data + pixel_pos->byte_offset;
  if (pixel_pos->is_first)
  {
    color->red = (*current_pixel_data & 0xf0) >> 4;
    color->green = *current_pixel_data & 0x0f;
    color->blue = (*(current_pixel_data + 1) & 0xf0) >> 4;
  }
  else
  {
    color->red = *current_pixel_data & 0x0f;
    color->green = (*(current_pixel_data + 1) & 0xf0) >> 4;
    color->blue = *(current_pixel_data + 1) & 0x0f;
  }
}

static void add_pixel_to_data(
  uint8_t *const pixel_data,
  led_panels_pixel_pos *const pixel_pos,
  const led_panels_color *const color
)
{
  uint8_t *current_pixel_data = pixel_data + pixel_pos->byte_offset;
  if (pixel_pos->is_first)
  {
    *current_pixel_data = 0x0;
    *current_pixel_data |= color->red << 4;
    *current_pixel_data |= color->green;
    *(current_pixel_data + 1) &= 0x0f;
    *(current_pixel_data + 1) |= color->blue << 4;
  }
  else
  {
    *current_pixel_data &= 0xf0;
    *current_pixel_data |= color->red;
    *(current_pixel_data + 1) = 0x0;
    *(current_pixel_data + 1) |= color->green << 4;
    *(current_pixel_data + 1) |= color->blue;
  }
}

static uint16_t get_panel_offset(
  uint8_t panel_index,
  led_panels_size *panels_sizes
)
{
  uint16_t pixels_offset = 0U;

  for (uint8_t i = 0; i < panel_index; i++)
    pixels_offset += GET_PANEL_SIZE(i, panels_sizes);

  return (uint16_t)(pixels_offset * 1.5f);
}

static led_panels_pixel_pos get_pixel_offset(
  uint8_t panel_index,
  led_panels_size *panels_sizes,
  uint8_t x,
  uint8_t y
)
{
  uint16_t side_size = get_side_size(
    GET_PANEL_SIZE(panel_index, panels_sizes)
  );

  // Correction for LED tape trajectory - every even line is inverted
  uint16_t pos_x = (y % 2 == 0 ? (side_size - 1 - x) : x);

  uint16_t pixel_num = pos_x + y * side_size;
  bool is_odd = pixel_num % 2 == 1;

  return (led_panels_pixel_pos) {
    .byte_offset = (pixel_num / 2) * 3 + (uint16_t)is_odd,
    .is_first = is_odd ? false : true
  };
}

static led_panels_status check_bounds(
  led_panels_buffer *buffer,
  uint8_t panel_index,
  uint8_t x,
  uint8_t y
)
{
  uint16_t panel_side_size = get_side_size(
    buffer->panels_sizes[panel_index]
  );

  if (panel_index >= buffer->panels_num)
    return LED_PANELS_BOUNDS;
  if (x >= panel_side_size || y >= panel_side_size)
    return LED_PANELS_BOUNDS;

  return LED_PANELS_OK;
}

// Implementations -----------------------------------------------------------

led_panels_buffer *led_panels_create(
  uint8_t panels_num,
  const led_panels_size *const panels_sizes
)
{
  uint16_t pixels_num = 0U;

  led_panels_buffer *buffer = malloc(sizeof(led_panels_buffer));
  buffer->is_locking = false;
  buffer->panels_num = panels_num;
  buffer->panels_sizes = calloc(panels_num, sizeof(led_panels_size));
  buffer->transmit_index = 0;
  memcpy(
    buffer->panels_sizes,
    panels_sizes,
    sizeof(led_panels_size) * panels_num
  );

  for (uint8_t i = 0; i < panels_num; i++)
    pixels_num += (uint16_t)panels_sizes[i];

  buffer->pwm_data = malloc(96); // 4 pixels
  memset(buffer->pwm_data, LED_PANELS_0_VALUE, 96 * sizeof(uint8_t));

  buffer->pixel_data_size = (uint16_t)(pixels_num * 1.5f);
  buffer->pixel_data = malloc(buffer->pixel_data_size);
  led_panels_flush(buffer);

  return buffer;
}

void led_panels_destroy(led_panels_buffer *buffer)
{
  if (buffer == NULL)
    return;
  free(buffer->panels_sizes);
  free(buffer->pwm_data);
  free(buffer->pixel_data);
  buffer->is_locking = false;
  buffer->transmit_index = 0;
  buffer->pixel_data_size = 0;
  free(buffer);
  buffer = NULL;
}

led_panels_status led_panels_get_pixel(
  led_panels_buffer *buffer,
  const uint8_t panel_index,
  const uint8_t pixel_x,
  const uint8_t pixel_y,
  led_panels_color *const pixel
)
{
  led_panels_status status = check_bounds(
    buffer, panel_index, pixel_x, pixel_y
  );
  if (status)
    return status;

  uint16_t panel_offset = get_panel_offset(
    panel_index,
    buffer->panels_sizes
  );

  led_panels_pixel_pos pixel_pos = get_pixel_offset(
    panel_index,
    buffer->panels_sizes,
    pixel_x,
    pixel_y
  );

  get_pixel_from_data(
    buffer->pixel_data + panel_offset,
    &pixel_pos,
    pixel
  );

  return LED_PANELS_OK;
}

led_panels_status led_panels_set_pixel(
  led_panels_buffer *buffer,
  const uint8_t panel_index,
  const uint8_t pixel_x,
  const uint8_t pixel_y,
  const led_panels_color pixel
)
{
  led_panels_status status = check_bounds(
    buffer, panel_index, pixel_x, pixel_y
  );
  if (status)
    return status;
  if (buffer->is_locking)
    return LED_PANELS_BUSY;

  uint16_t panel_offset = get_panel_offset(
    panel_index,
    buffer->panels_sizes
  );

  led_panels_pixel_pos pixel_pos = get_pixel_offset(
    panel_index,
    buffer->panels_sizes,
    pixel_x,
    pixel_y
  );

  add_pixel_to_data(
    buffer->pixel_data + panel_offset,
    &pixel_pos,
    &pixel
  );

  return LED_PANELS_OK;
}

led_panels_status led_panels_flush(led_panels_buffer *buffer)
{
  if (buffer->is_locking)
    return LED_PANELS_BUSY;

  memset(
    buffer->pixel_data,
    0,
    buffer->pixel_data_size
  );

  return LED_PANELS_OK;
}

led_panels_status led_panels_send(led_panels_buffer *buffer)
{
  if (buffer->is_locking)
    return LED_PANELS_BUSY;

  // First part of pwm data
  set_pixels_to_pwm_data(
    buffer->pwm_data,
    buffer->pixel_data,
    &buffer->transmit_index
  );

  // Second part of pwm data
  set_pixels_to_pwm_data(
    buffer->pwm_data + 48,
    buffer->pixel_data,
    &buffer->transmit_index
  );

  led_panels_status status = led_panels_io_send_data(
    buffer->pwm_data,
    96 * sizeof(uint8_t)
  );

  if (status == LED_PANELS_OK)
    buffer->is_locking = true;

  return status;
}

void led_panels_copy_data(
  led_panels_buffer *target,
  led_panels_buffer *source
)
{
  memcpy(
    target->pixel_data,
    source->pixel_data,
    target->pixel_data_size
  );
}

void led_panels_send_complete(led_panels_buffer *buffer)
{
  if (buffer->transmit_index > buffer->pixel_data_size)
  {
    led_panels_io_stop_sending_data();
    buffer->is_locking = false;
    buffer->transmit_index = 0;
    memset(buffer->pwm_data, LED_PANELS_0_VALUE, 96 * sizeof(uint8_t));
    return;
  }

  // Reset (above 50 us)
  if (buffer->transmit_index >= buffer->pixel_data_size)
  {
    memset(buffer->pwm_data + 48, LED_PANELS_RESET_VALUE, 48 * sizeof(uint8_t));
    buffer->transmit_index += 6;
    return;
  }

  // Second part of pwm data
  set_pixels_to_pwm_data(
    buffer->pwm_data + 48,
    buffer->pixel_data,
    &buffer->transmit_index
  );
}

void led_panels_half_send_complete(led_panels_buffer *buffer)
{
  // Reset (above 50 us)
  if (buffer->transmit_index >= buffer->pixel_data_size)
  {
    memset(buffer->pwm_data, LED_PANELS_RESET_VALUE, 48 * sizeof(uint8_t));
    return;
  }

  // First part of pwm data
  set_pixels_to_pwm_data(
    buffer->pwm_data,
    buffer->pixel_data,
    &buffer->transmit_index
  );
}
