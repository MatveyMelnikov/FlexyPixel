#include "led_panels_driver.h"
#include "led_panels_defs.h"
#include "led_panels_io.h"
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include <math.h>

// Defines -------------------------------------------------------------------

#define PWM_PIXEL_SIZE 24
#define GET_PANEL_SIZE(index, sizes) \
  (uint16_t)(sizes[index])

// Static variables ----------------------------------------------------------

// Static functions ----------------------------------------------------------

static void convert_color_to_pwm_data(
  uint8_t *const pwm_data,
  const uint8_t *const color
)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    *(pwm_data + i) = (bool)(*color & (1 << (7 - i))) ? 
      LED_PANELS_1_VALUE : LED_PANELS_0_VALUE;
  }
}

static void convert_pwm_data_to_color(
  const uint8_t *const pwm_data,
  uint8_t *const color
)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    *color |= (
      *(pwm_data + (7 - i)) == LED_PANELS_1_VALUE ? 1U : 0U
    ) << i;
  }
}

__attribute__((always_inline))
inline static uint16_t get_side_size(uint16_t size)
{
  static const uint8_t sides_sizes[] = { 8U, 16U };
  return sides_sizes[size >> 8]; // size / 256
}

static void convert_pwm_data_to_pixel(
  uint8_t *const pwm_data,
  led_panels_color *const pixel
)
{
  convert_pwm_data_to_color(pwm_data, &pixel->green);
  convert_pwm_data_to_color(pwm_data + 8, &pixel->red);
  convert_pwm_data_to_color(pwm_data + 16, &pixel->blue);
}

static void convert_pixel_to_pwm_data(
  uint8_t *const pwm_data,
  led_panels_color *const pixel
)
{
  convert_color_to_pwm_data(pwm_data, &pixel->green);
  convert_color_to_pwm_data(pwm_data + 8, &pixel->red);
  convert_color_to_pwm_data(pwm_data + 16, &pixel->blue);
}

static uint16_t get_panel_offset(
  uint8_t panel_index,
  led_panels_size *panels_sizes
)
{
  uint16_t pixels_offset = 0U;

  uint8_t i = 0;
  for (; i < panel_index; i++)
  pixels_offset += GET_PANEL_SIZE(i, panels_sizes);

  return pixels_offset * PWM_PIXEL_SIZE;
}

static uint16_t get_pixel_offset(
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
  uint16_t pixel_pos = (y % 2 == 0 ? (7 - x) : x) + y * side_size;
  return pixel_pos * PWM_PIXEL_SIZE;
}

static uint16_t get_pwm_data_size(led_panels_buffer *buffer)
{
  uint16_t result = 0U;

  uint8_t i = 0;
  for (; i < buffer->panels_num; i++)
    result += GET_PANEL_SIZE(i, buffer->panels_sizes);

  return result * 3 * 8 + 50; 
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
  memcpy(
    buffer->panels_sizes,
    panels_sizes,
    sizeof(led_panels_size) * panels_num
  );

  for (uint8_t i = 0; i < panels_num; i++)
    pixels_num += (uint16_t)panels_sizes[i];

  uint16_t total_size = pixels_num * 3 * 8 + 50;
  buffer->pwm_data = calloc(total_size, sizeof(uint8_t));
  memset(
    buffer->pwm_data + (total_size - 50),
    LED_PANELS_RESET_VALUE,
    50 * sizeof(uint8_t)
  ); // reset (above 50 us)
  led_panels_flush(buffer);

  return buffer;
}

void led_panels_destroy(led_panels_buffer *buffer)
{
  free(buffer->panels_sizes);
  free(buffer->pwm_data);
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
  uint8_t *panel = buffer->pwm_data + panel_offset;
  uint16_t pwm_positions_offset = pixel_x * 3 + 
    (pixel_y * 3 * (uint16_t)buffer->panels_sizes[panel_index]);

  convert_pwm_data_to_pixel(panel + pwm_positions_offset, pixel);

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
  uint16_t pixel_offset = get_pixel_offset(
    panel_index,
    buffer->panels_sizes,
    pixel_x,
    pixel_y
  );

  convert_pixel_to_pwm_data(
    buffer->pwm_data + panel_offset + pixel_offset,
    (led_panels_color *const)&pixel
  );

  return LED_PANELS_OK;
}

led_panels_status led_panels_flush(led_panels_buffer *buffer)
{
  if (buffer->is_locking)
    return LED_PANELS_BUSY;

  memset(
    buffer->pwm_data,
    LED_PANELS_0_VALUE,
    get_pwm_data_size(buffer) - 50
  );

  return LED_PANELS_OK;
}

led_panels_status led_panels_send(led_panels_buffer *buffer)
{
  if (buffer->is_locking)
    return LED_PANELS_BUSY;

  led_panels_status status = led_panels_io_send_data(
    buffer->pwm_data,
    get_pwm_data_size(buffer)
  );

  if (status == LED_PANELS_OK)
    buffer->is_locking = true;

  return status;
}

void led_panels_send_complete(led_panels_buffer *buffer)
{
  buffer->is_locking = false;
}
