#include "set_mode_handler.h"
#include "render_controller_io.h"
#include "render_controller_defs.h"
#include "displays_conf.h"
#include "handler_list.h"
#include <stdlib.h>

// Static functions ----------------------------------------------------------

static void set_handlers(handler self, handler_input *const input)
{
  handler mode = handler_list_get_mode();

  if (mode == NULL || displays_conf_is_empty())
  {
    hc06_write((uint8_t *)UNCONFIGURED_STRING, strlen(UNCONFIGURED_STRING));
    hc06_read(input->data, CMD_LEN);
    return;
  }

  handler_set(mode, input);
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

handler send_data_handler_create()
{
  handler handler = malloc(sizeof(handler_struct));
  handler->name = "DATA";
  handler->vtable = &interface;

  return handler;
}
