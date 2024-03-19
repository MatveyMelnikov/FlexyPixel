#include "pixel_mode_handler.h"
#include "hc06_driver.h"
#include "render_controller.h"
#include "render_controller_defs.h"
#include "handler_queue.h"
#include "led_panels_driver.h"
#include "list_of_changes.h"
#include "displays_conf.h"
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
  return (displays_conf_is_empty() || is_first_field || is_last_field);
}

static void handle_pixel_data(handler_input *const input)
{
  // Symbols: 60
  // {"panelPosition":9,"pixelPosition":"000","pixelColor":"967"}

  render_controller_io_stop_timeout_timer();
  if (is_transmit_wrong(input))
    END_HANDLE_WITH_ERROR();

  uint8_t panel_position = CHAR_TO_NUM(input->data + 17);
  uint8_t pixel_position = STR_TO_NUM(input->data + 36);
  led_panels_color color = (led_panels_color) {
    .red = CHAR_TO_NUM(input->data + 55),
    .green = CHAR_TO_NUM(input->data + 56),
    .blue = CHAR_TO_NUM(input->data + 57)
  };

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