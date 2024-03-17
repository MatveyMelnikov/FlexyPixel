#include "pixel_mode_handler.h"
#include "hc06_driver.h"
#include "render_controller.h"
#include "render_controller_defs.h"
#include "handler_queue.h"
#include "led_panels_driver.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// Defines -------------------------------------------------------------------

#define END_HANDLE_WITH_ERROR() \
  { \
    hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING)); \
    return; \
  }

// Static functions ----------------------------------------------------------

static bool is_transmit_wrong(handler_input *const input)
{
  bool is_first_field = !CHECK_STR(input->data + 2, "panelPosition", 13);
  bool is_last_field = !CHECK_STR(input->data + 42, "pixelColor", 10);
  return (input->configurations[0] == 0) || is_first_field || is_last_field;
}

static void handle_pixel_data(handler_input *const input)
{
  // Symbols: 60
  // {"panelPosition":9,"pixelPosition":"000","pixelColor":"967"}

  render_controller_io_stop_timeout_timer();
  if (is_transmit_wrong(input))
    END_HANDLE_WITH_ERROR();

  uint8_t panel_position = *(input->data + 17) - '0';
  uint8_t pixel_position = STR_TO_NUM(input->data + 36);
  led_panels_color color = (led_panels_color) {
    .red = *(input->data + 55) - '0',
    .green = *(input->data + 56) - '0',
    .blue = *(input->data + 57) - '0',
  };

  bool is_panel_not_configured =
    input->configurations[panel_position] == LED_PANELS_UNKNOWN;
  if (is_panel_not_configured)
    END_HANDLE_WITH_ERROR();

  uint16_t panel_size = get_side_size(
    input->configurations[panel_position]
  );
  led_panels_status status = led_panels_set_pixel(
    *input->buffer,
    panel_position,
    pixel_position % panel_size,
    pixel_position / panel_size,
    color
  );
  
  send_status(status == LED_PANELS_OK);
}

void set_handlers(mode_handler self, handler_input *const input)
{
  handler_queue_clear();
  handler_queue_add(handle_pixel_data);

  hc06_read((uint8_t*)input->data, 60);
  render_controller_io_start_timeout_timer();
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