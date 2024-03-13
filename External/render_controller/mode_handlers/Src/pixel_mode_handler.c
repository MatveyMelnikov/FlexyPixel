#include "pixel_mode_handler.h"
#include "hc06_driver.h"
#include "render_controller_defs.h"
#include "handler_queue.h"
#include "led_panels_driver.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// Static functions ----------------------------------------------------------

static void handle_pixel_data(handler_input *const input)
{
  // Symbols: 60
  // {"panelPosition":9,"pixelPosition":"000","pixelColor":"967"}

  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  uint8_t panel_position = *(input->data + 17) - '0';
  uint8_t pixel_position = STR_TO_NUM(input->data + 36);
  led_panels_color color = (led_panels_color) {
    .red = *(input->data + 55) - '0',
    .green = *(input->data + 56) - '0',
    .blue = *(input->data + 57) - '0',
  };
  uint16_t panel_size = get_side_size(
    input->configurations[panel_position]
  );

  led_panels_status status = led_panels_set_pixel(
    input->buffer,
    panel_position,
    pixel_position % panel_size,
    pixel_position / panel_size,
    color
  );

  if (status)
  {
    hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
    return;
  }
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));
}

void set_handlers(mode_handler self, handler_input *const input)
{
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  handler_queue_clear();
  handler_queue_add(handle_pixel_data);

  hc06_read((uint8_t*)input->data, 60);
}

void destroy(mode_handler self)
{
  free(self);
}

// Static variables ----------------------------------------------------------

static mode_handler_interface_struct interface =
{
  set_handlers,
  destroy
};

// Implementations -----------------------------------------------------------

mode_handler pixel_mode_handler_create()
{
  mode_handler handler = malloc(sizeof(mode_handler_struct));
  handler->mode_name = "PIX";
  handler->vtable = &interface;

  return handler;
}