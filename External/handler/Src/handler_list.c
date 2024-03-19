#include "handler_list.h"
#include <stddef.h>
#include <memory.h>
#include <string.h>

// Static variables ----------------------------------------------------------

static handler handlers[HANDLERS_NUM] = { NULL };
static handler current_mode = NULL;
static uint8_t top_index = 0;

// Implementations -----------------------------------------------------------

void handler_list_add(handler *const new_handlers, uint8_t handlers_num)
{
  if (top_index * handlers_num >= HANDLERS_NUM)
    return;

  top_index += handlers_num;
  memcpy(handlers, new_handlers, handlers_num * sizeof(handler));
}

void handler_list_set_mode(uint8_t mode_index)
{
  if (mode_index >= top_index)
    return;
  current_mode = handlers[mode_index];
}

handler handler_list_get_mode()
{
  return current_mode;
}

void handler_list_destroy()
{
  top_index = 0;
  current_mode = NULL;
}

const char *handler_list_get_name(uint8_t index)
{
  if (index >= top_index)
    return NULL;

  return handler_get_name(handlers[index]);
}

void handler_list_set(uint8_t index, handler_input *const input)
{
  if (index >= top_index)
    return;

  handler_set(handlers[index], input);
}
