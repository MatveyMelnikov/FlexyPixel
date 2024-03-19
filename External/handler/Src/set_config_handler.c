#include "set_config_handler.h"
#include "render_controller_io.h"
#include "render_controller_defs.h"
#include "displays_conf.h"
#include "handler_queue.h"
#include <stdlib.h>

// Static functions ----------------------------------------------------------

static void handle_configuration(handler_input *const input)
{
  // 73 symbols
  // {"configuration":["256","256","256","256","256","256","256","256","256"]}

  render_controller_io_stop_timeout_timer();
  if (is_first_field_not_suitable(input->data, "configuration"))
    return;

  bool is_ok = true;
  led_panels_size new_configuration[CONFIGURATION_SIZE] = { 0 };
  uint8_t displays_num = 0;
  for (; displays_num < CONFIGURATION_SIZE; displays_num++)
  {
    uint16_t display_size = STR_TO_NUM(
      (char *)input->data + CONFIGURATION_OFFSET + (6 * displays_num)
    );
    if (display_size == 0)
      break;
    if (display_size != LED_PANELS_SIZE_64 && 
      display_size != LED_PANELS_SIZE_256)
    {
      is_ok = false;
      break;
    }

    new_configuration[displays_num] = display_size;
  }

  if (is_ok)
    displays_conf_update(new_configuration, displays_num);
  send_status(is_ok);
}

static void set_handlers(handler self, handler_input *const input)
{
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  handler_queue_clear();
  handler_queue_add(handle_configuration);

  hc06_read(input->data, 73);
  render_controller_io_start_timeout_timer();
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
