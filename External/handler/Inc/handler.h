#ifndef HANDLER_H
#define HANDLER_H

#include <stdint.h>
#include "handler_input.h"

typedef struct handler_struct * handler;

void handler_destroy(handler self);
void handler_set(
  handler self,
  handler_input *const input
);
char *const handler_get_name(handler self);

// Structures handler_struct and handler_interface_struct
// are declared here:
#include "handler_private.h"

#endif
