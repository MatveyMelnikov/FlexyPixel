#ifndef HANDLER_INPUT_H
#define HANDLER_INPUT_H

#include <stdint.h>
#include "led_panels_defs.h"

typedef struct
{
  uint8_t *const data;
  led_panels_buffer *const buffer;
  uint16_t *configurations;
} handler_input;

#endif
