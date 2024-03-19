#ifndef HANDLER_PRIVATE_H
#define HANDLER_PRIVATE_H

#include <stdint.h>
#include "led_panels_defs.h"

typedef struct handler_interface_struct * handler_interface;

typedef struct handler_struct
{
  char *name;
  handler_interface vtable;
} handler_struct;

typedef struct handler_interface_struct
{
  void (*set_handlers)(handler, handler_input *const);
  void (*destroy)(handler);
} handler_interface_struct;

#endif
