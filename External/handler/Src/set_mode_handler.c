#include "set_mode_handler.h"
#include "render_controller_io.h"
#include "render_controller_defs.h"
#include "displays_conf.h"
#include "handler_list.h"
#include "handler_queue.h"
#include <stdlib.h>

// Static functions ----------------------------------------------------------

static void handle_mode(handler_input *const input)
{
  // 14 symbols
  // {"mode":"SEQ"} (PIX)

  render_controller_io_stop_timeout_timer();
  if (is_first_field_not_suitable(input->data, "mode"))
    return;

  bool is_ok = false;

  const char *handler_name = NULL;
  for (uint8_t i = 0; i < HANDLERS_NUM; i++)
  {
    handler_name = handler_list_get_name(i);
    if (handler_name == NULL)
      break;
    
    if (CHECK_STR(input->data + MODE_OFFSET, handler_name, MODE_LEN))
    {
      handler_list_set_mode(i);
      is_ok = true;
      break;
    }
  }

  send_status(is_ok);
}

static void set_handlers(handler self, handler_input *const input)
{
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  handler_queue_clear();
  handler_queue_add(handle_mode);

  hc06_read(input->data, 14);
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

handler set_mode_handler_create()
{
  handler handler = malloc(sizeof(handler_struct));
  handler->name = "mode";
  handler->vtable = &interface;

  return handler;
}
