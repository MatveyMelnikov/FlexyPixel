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

// Variables -----------------------------------------------------------------

enum {
  RED_OFFSET = 33U,
  GREEN_OFFSET = 34U,
  BLUE_OFFSET = 35U,
  PANEL_POS_OFFSET = 17U,
  PIXEL_POS_OFFSET = 55,
  PANEL_POS_FIELD_OFFSET = 2U,
  PIXEL_POS_FIELD_OFFSET = 39U,
  PIXEL_CMD_LENGTH = 60U,
};

// Defines -------------------------------------------------------------------

#define END_HANDLE_WITH_ERROR() \
  { \
    hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING)); \
    return; \
  }

// Static functions ----------------------------------------------------------

static bool is_transmit_wrong(handler_input *const input)
{
  bool is_first_field = !CHECK_STR(
    input->data + PANEL_POS_FIELD_OFFSET,
    "panelPosition",
    sizeof("panelPosition")
  );
  bool is_last_field = !CHECK_STR(
    input->data + PIXEL_POS_FIELD_OFFSET,
    "pixelPosition",
    sizeof("pixelPosition")
  );
  return (displays_conf_is_empty() || is_first_field || is_last_field);
}

static void handle_pixel_data(handler_input *const input)
{
  // Symbols: 60
  // {"panelPosition":9,"pixelColor":"967","pixelPosition":"000"}

  render_controller_io_stop_timeout_timer();
  if (is_transmit_wrong(input))
    END_HANDLE_WITH_ERROR();

  uint8_t panel_position = CHAR_TO_NUM(input->data + PANEL_POS_OFFSET);
  led_panels_color color = (led_panels_color) {
    .red = CHAR_TO_NUM(input->data + RED_OFFSET),
    .green = CHAR_TO_NUM(input->data + GREEN_OFFSET),
    .blue = CHAR_TO_NUM(input->data + BLUE_OFFSET)
  };
  uint8_t pixel_position = STR_TO_NUM(input->data + PIXEL_POS_OFFSET);

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
  handler_queue_clear();
  handler_queue_add(handle_pixel_data);

  hc06_read((uint8_t*)input->data, PIXEL_CMD_LENGTH);
  render_controller_io_start_timeout_timer();

  hc06_write((uint8_t*)OK_STRING, strlen(OK_STRING));
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