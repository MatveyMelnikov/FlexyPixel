#include "operation_mode.h"
#include <stddef.h>

// Static variables ----------------------------------------------------------

static mode_handler current_mode = NULL;

// Implementations -----------------------------------------------------------

void operation_mode_set(mode_handler mode)
{
  current_mode = mode;
}

void operation_mode_clear()
{
  current_mode = NULL;
}

bool operation_mode_is_set()
{
  return current_mode != NULL;
}

mode_handler operation_mode_get()
{
  return current_mode;
}
