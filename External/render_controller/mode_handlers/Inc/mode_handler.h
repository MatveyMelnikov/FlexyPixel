#ifndef MODE_HANDLER_H
#define MODE_HANDLER_H

#include <stdint.h>
#include "led_panels_defs.h"

typedef struct mode_handler_struct * mode_handler;

void mode_handler_destroy(mode_handler self);
void mode_handler_set_handlers(
  mode_handler self
);
uint16_t mode_handler_get_render_delay(mode_handler self);
char *const mode_handler_get_name(mode_handler self);

// Structures mode_handler_struct and mode_handler_interface_struct
// are declared here:
#include "mode_handler_private.h"

#endif
