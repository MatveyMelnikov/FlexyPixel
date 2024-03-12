#ifndef LED_PANELS_DRIVER_H
#define LED_PANELS_DRIVER_H

#include <stdint.h>
#include "led_panels_defs.h"

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
// Called in the end of transfer handler
void led_panels_send_complete(led_panels_buffer *buffer);

#endif
