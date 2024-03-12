#ifndef MODE_HANDLER_PRIVATE_H
#define MODE_HANDLER_PRIVATE_H

#include <stdint.h>
#include "led_panels_defs.h"

typedef struct mode_handler_interface_struct * mode_handler_interface;

typedef struct mode_handler_struct
{
  uint16_t render_delay;
  char *mode_name;
  mode_handler_interface vtable;
} mode_handler_struct;

typedef struct mode_handler_interface_struct
{
  void (*set_handlers)(mode_handler, uint8_t *);
  void (*destroy)(mode_handler);
} mode_handler_interface_struct;

#endif
