#include "mode_handler.h"
#include <stddef.h>

// Implementations -----------------------------------------------------------

void mode_handler_destroy(mode_handler self)
{
  self->vtable->destroy(self);
}

void mode_handler_set_handlers(
  mode_handler self,
  handler_input *const input
)
{
  self->vtable->set_handlers(self, input);
}

char *const mode_handler_get_name(mode_handler self)
{
  return self->mode_name;
}
