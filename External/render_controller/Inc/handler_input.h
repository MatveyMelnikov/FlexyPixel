#ifndef HANDLER_INPUT_H
#define HANDLER_INPUT_H

#include <stdint.h>
#include "led_panels_defs.h"

typedef struct
{
  uint8_t *data;
  led_panels_buffer *buffer;
  led_panels_size *configurations;
} handler_input;

#endif
