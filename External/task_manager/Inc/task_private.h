#ifndef TASK_PRIVATE_H
#define TASK_PRIVATE_H

#include <stdint.h>
#include <stdbool.h>
#include "task_output.h"
#include "task_input.h"

typedef struct task_interface_struct * task_interface;

typedef struct task_struct
{
  char *name;
  task_interface vtable;
  uint32_t ttl; // 0 - not limited
  uint8_t subtasks_amount;
} task_struct;

typedef struct task_interface_struct
{
  task_output (**subtasks)(task_input *const);
  void (*destroy)(task);
} task_interface_struct;

#endif
