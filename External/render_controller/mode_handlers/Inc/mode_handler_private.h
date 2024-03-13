#ifndef MODE_HANDLER_PRIVATE_H
#define MODE_HANDLER_PRIVATE_H

#include <stdint.h>
#include "led_panels_defs.h"

typedef struct mode_handler_interface_struct * mode_handler_interface;

typedef struct mode_handler_struct
{
  char *mode_name;
  mode_handler_interface vtable;
} mode_handler_struct;

typedef struct mode_handler_interface_struct
{
  void (*set_handlers)(mode_handler, handler_input *const);
  void (*destroy)(mode_handler);
} mode_handler_interface_struct;

#endif
