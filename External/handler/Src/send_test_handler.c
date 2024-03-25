#include "send_test_handler.h"
#include "render_controller_io.h"
#include "render_controller_defs.h"
#include "displays_conf.h"
#include "handler_list.h"
#include "debug_output.h"
#include <stdlib.h>

// Static functions ----------------------------------------------------------

static void set_handlers(handler self, handler_input *const input)
{
  render_controller_io_stop_timeout_timer();

  memset(input->data, 'F', 500);
  hc06_write(input->data, 500);

  DEBUG_OUTPUT("test sent", strlen("test sen"));
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

handler send_test_handler_create()
{
  handler handler = malloc(sizeof(handler_struct));
  handler->name = "TEST";
  handler->vtable = &interface;

  return handler;
}
