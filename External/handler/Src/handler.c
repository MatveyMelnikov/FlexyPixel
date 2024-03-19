#include "handler.h"
#include <stddef.h>

// Implementations -----------------------------------------------------------

void handler_destroy(handler self)
{
  self->vtable->destroy(self);
}

void handler_set(
  handler self,
  handler_input *const input
)
{
  self->vtable->set_handlers(self, input);
}

char *const handler_get_name(handler self)
{
  return self->name;
}
