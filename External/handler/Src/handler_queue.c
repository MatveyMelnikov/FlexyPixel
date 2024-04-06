#include "handler_queue.h"
#include <stddef.h>
#include <stdbool.h>
#include <memory.h>

// Static variables ----------------------------------------------------------

enum {
  QUEUE_SIZE = 5U
};

static void (*handlers[QUEUE_SIZE])(handler_input *const) = {
  NULL
};
static uint8_t top_index = 0;
static uint8_t current_index = 0;
static bool hold_flag = false;
static bool skip_remove = false;

// Implementations -----------------------------------------------------------

bool handler_queue_add(
  void (*handle_function)(handler_input *const input)
)
{
  if (top_index >= 5)
    return false;
  
  handlers[top_index] = handle_function;
  top_index++;

  return true;
}

bool handler_queue_run(handler_input *const input)
{
  if (handler_queue_is_empty())
    return false;

  handlers[current_index](input);

  if (hold_flag)
    return true;

  if (skip_remove)
    skip_remove = false;
  else
    current_index++;

  // finish handling
  if (handler_queue_is_empty())
    handler_queue_clear();

  return true;
}

bool handler_queue_is_empty()
{
  return current_index >= top_index;
}

void handler_queue_clear()
{
  memset(handlers, 0, sizeof(uint32_t) * QUEUE_SIZE);
  current_index = 0;
  top_index = 0;
  hold_flag = false;
}

void handler_queue_set_hold(bool is_hold)
{
  hold_flag = is_hold;
}

bool handler_queue_get_hold_flag()
{
  return hold_flag;
}

void handler_queue_skip_remove()
{
  skip_remove = true;
}
