#include "pixel_mode_handler.h"
#include "hc06_driver.h"
#include "render_controller_io.h"
#include "render_controller_defs.h"
#include "handler_queue.h"
#include "led_panels_driver.h"
#include "list_of_changes.h"
#include "displays_conf.h"
#include "debug_output.h"
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
  bool is_last_field = !CHECK_STR(input->data + 39, "pixelPosition", 13);
  return (displays_conf_is_empty() || is_first_field || is_last_field);
}

static void handle_pixel_data(handler_input *const input)
{
  // Symbols: 60
  // {"panelPosition":9,"pixelColor":"967","pixelPosition":"000"}

  render_controller_io_stop_timeout_timer();
  if (is_transmit_wrong(input))
    END_HANDLE_WITH_ERROR();

  uint8_t panel_position = CHAR_TO_NUM(input->data + 17);
  led_panels_color color = (led_panels_color) {
    .red = CHAR_TO_NUM(input->data + 33),
    .green = CHAR_TO_NUM(input->data + 34),
    .blue = CHAR_TO_NUM(input->data + 35)
  };
  uint8_t pixel_position = STR_TO_NUM(input->data + 55);

  if (!displays_conf_is_panel_configured(panel_position))
    END_HANDLE_WITH_ERROR();

  uint16_t panel_size = get_side_size(displays_conf_get()[panel_position]);

  bool is_ok = list_of_changes_add(
    (pixel_change){
      .color = color,
      .panel_position = panel_position,
      .x = pixel_position % panel_size,
      .y = pixel_position / panel_size
    }
  );
  
  send_status(is_ok);
}

static void set_handlers(handler self, handler_input *const input)
{
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  handler_queue_clear();
  handler_queue_add(handle_pixel_data);

  hc06_read((uint8_t*)input->data, 60);
  render_controller_io_start_timeout_timer();

  DEBUG_OUTPUT("pixel mode handler set", strlen("pixel mode handler set"));
}

static void destroy(handler self)
{
  free(self);
}

// Static variables ----------------------------------------------------------

static handler_interface_struct interface =
{
  set_handlers,
  destroy
};

// Implementations -----------------------------------------------------------

handler pixel_mode_handler_create()
{
  handler handler = malloc(sizeof(handler_struct));
  handler->name = "PIX";
  handler->vtable = &interface;

  return handler;
}