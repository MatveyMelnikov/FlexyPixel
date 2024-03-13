#ifndef HANDLER_QUEUE_H
#define HANDLER_QUEUE_H

#include <stdint.h>
#include "handler_input.h"

bool handler_queue_add(
  void (*handle_function)(handler_input *const)
);
bool handler_queue_run(handler_input *const input);
bool handler_queue_is_empty(void);
void handler_queue_clear(void);

#endif
