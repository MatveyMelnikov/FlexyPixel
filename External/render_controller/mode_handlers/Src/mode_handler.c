#include "mode_handler.h"
#include <stddef.h>

// Implementations -----------------------------------------------------------

void mode_handler_destroy(mode_handler self)
{
  self->vtable->destroy(self);
}

void mode_handler_set_handlers(
  mode_handler self
)
{
  self->vtable->set_handlers(self);
}

uint16_t mode_handler_get_render_delay(mode_handler self)
{
  return self->render_delay;
}

char *const mode_handler_get_name(mode_handler self)
{
  return self->mode_name;
}
