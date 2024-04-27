#include "set_config_handler.h"
#include "render_controller_io.h"
#include "render_controller_defs.h"
#include "displays_conf.h"
#include "handler_queue.h"
#include "debug_output.h"
#include "list_of_changes.h"
#include <stdlib.h>

// Variables -----------------------------------------------------------------

enum
{
  CONF_OFFSET = 18U,
  CONF_CMD_LENGTH = 47U,
};

// Static functions ----------------------------------------------------------

static void handle_configuration(handler_input *const input)
{
  // 47 symbols
  // {"configuration":"256064000000000000000000000"}

  bool is_ok = true;
  render_controller_io_stop_timeout_timer();
  if (is_first_field_not_suitable(input->data, "configuration"))
  {
    is_ok = false;
    goto handle_end;
  }

  led_panels_size new_configuration[CONFIGURATION_SIZE] = { 0 };
  uint8_t displays_num = 0;
  for (; displays_num < CONFIGURATION_SIZE; displays_num++)
  {
    uint16_t display_size = STR_TO_NUM(
      (char *)input->data + CONF_OFFSET + (3 * displays_num)
    );
    if (display_size == 0)
      break;
    if (
      display_size != LED_PANELS_SIZE_64 && 
      display_size != LED_PANELS_SIZE_256
    )
    {
      is_ok = false;
      break;
    }

    new_configuration[displays_num] = display_size;
    list_of_changes_clear();
  }

handle_end:
  if (is_ok)
    displays_conf_update(new_configuration, displays_num);
  send_status(is_ok);
}

static void set_handlers(handler self, handler_input *const input)
{
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  handler_queue_clear();
  handler_queue_add(handle_configuration);

  hc06_read(input->data, CONF_CMD_LENGTH);
  render_controller_io_start_timeout_timer();

  DEBUG_OUTPUT("config handler set", strlen("config handler set"));
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

handler set_config_handler_create()
{
  handler handler = malloc(sizeof(handler_struct));
  handler->name = "CONF";
  handler->vtable = &interface;

  return handler;
}
