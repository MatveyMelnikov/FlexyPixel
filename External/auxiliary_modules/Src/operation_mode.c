#include "operation_mode.h"
#include <stddef.h>

// Static variables ----------------------------------------------------------

static handler current_mode = NULL;

// Implementations -----------------------------------------------------------

void operation_mode_set(handler mode)
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

handler operation_mode_get()
{
  return current_mode;
}
