#ifndef LED_PANELS_DRIVER_H
#define LED_PANELS_DRIVER_H

#include <stdint.h>
#include "led_panels_defs.h"

__attribute__((always_inline))
inline static uint16_t get_side_size(uint16_t size)
{
  static const uint8_t sides_sizes[] = { 8U, 16U };
  return sides_sizes[size >> 8]; // size / 256
}

led_panels_buffer *led_panels_create(
  uint8_t panels_num,
  const led_panels_size *const panels_sizes
);
void led_panels_destroy(led_panels_buffer *buffer);
led_panels_status led_panels_get_pixel(
  led_panels_buffer *buffer,
  const uint8_t panel_index,
  const uint8_t pixel_x,
  const uint8_t pixel_y,
  led_panels_color *const pixel
);
led_panels_status led_panels_set_pixel(
  led_panels_buffer *buffer,
  const uint8_t panel_index,
  const uint8_t pixel_x,
  const uint8_t pixel_y,
  const led_panels_color pixel
);
led_panels_status led_panels_flush(
  led_panels_buffer *buffer
);
led_panels_status led_panels_send(led_panels_buffer *buffer);
void led_panels_copy_data(
  led_panels_buffer *target,
  led_panels_buffer *source
);
// Called in the end of transfer handler
void led_panels_send_complete(led_panels_buffer *buffer);
void led_panels_half_send_complete(led_panels_buffer *buffer);

#endif
