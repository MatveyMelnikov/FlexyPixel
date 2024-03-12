#ifndef HANDLER_QUEUE_H
#define HANDLER_QUEUE_H

#include <stdint.h>
#include "led_panels_defs.h"

bool handler_queue_add(
  void (*handle_function)(uint8_t *const, led_panels_buffer *)
);
bool handler_queue_run(uint8_t *const data, led_panels_buffer *buffer);
bool handler_queue_is_empty(void);
void handler_queue_clear(void);

#endif
