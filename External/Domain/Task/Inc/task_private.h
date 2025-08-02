#ifndef TASK_PRIVATE_H
#define TASK_PRIVATE_H

#include <stdint.h>
#include <stdbool.h>
#include "task_defs.h"

typedef struct task_interface_struct * task_interface;

typedef struct task_struct
{
  task_interface vtable;
  uint32_t ttl; // in ticks
  uint32_t start_tick;
  bool is_infinite;
} task_struct;

typedef struct task_interface_struct
{
  task_output (*run)(task_arg *const);
  void (*destroy)(task_ptr);
} task_interface_struct;

#endif
