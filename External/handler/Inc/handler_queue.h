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
// Sets the value of the hold flag. It is responsible for
// whether the handlerwill be removed upon completion of its work
void handler_queue_set_hold(bool is_hold);
// Hold flag is responsible for
// whether the handlerwill be removed upon completion of its work
bool handler_queue_get_hold_flag(void);
// Single bypasses removing a handler from the queue.
// Next time it will work again
void handler_queue_skip_remove(void);

#endif
