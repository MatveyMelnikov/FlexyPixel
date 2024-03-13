#ifndef MODE_HANDLER_H
#define MODE_HANDLER_H

#include <stdint.h>
#include "handler_input.h"

typedef struct mode_handler_struct * mode_handler;

void mode_handler_destroy(mode_handler self);
void mode_handler_set_handlers(
  mode_handler self,
  handler_input *const input
);
char *const mode_handler_get_name(mode_handler self);

// Structures mode_handler_struct and mode_handler_interface_struct
// are declared here:
#include "mode_handler_private.h"

#endif
