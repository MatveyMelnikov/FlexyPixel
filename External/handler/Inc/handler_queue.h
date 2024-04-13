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
// Enable hold handler - it will not be removed from the handler queue
// until the flag is cleared
void handler_queue_set_hold(bool is_hold);
// The hold flag keeps the handler from being deleted
bool handler_queue_get_hold_flag(void);
// Single omission of handler removal after calling it
void handler_queue_skip_remove(void);

#endif
