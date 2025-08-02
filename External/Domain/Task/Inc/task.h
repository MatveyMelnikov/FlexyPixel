#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <stdbool.h>
#include "task_defs.h"

typedef struct task_struct * task_ptr;
typedef struct task_struct task;

void task_destroy(task_ptr self);
task_output task_execute(
  task_ptr self,
  task_arg *const argument
);
uint32_t task_get_ttl(task_ptr self);
uint32_t task_get_start_tick(task_ptr self);
void task_set_start_tick(task_ptr self, uint32_t tick);
bool task_is_infinite(task_ptr self);

// Structures task_struct and task_interface_struct
// are declared here:
#include "task_private.h"

#endif
