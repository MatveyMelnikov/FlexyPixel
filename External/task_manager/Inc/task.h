#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <stdbool.h>
#include "task_input.h"
#include "task_output.h"

typedef struct task_struct * task;

void task_destroy(task self);
task_output task_execute_subtask(
  task self,
  task_input *const input,
  uint8_t subtask_index
);
char *const task_get_name(task self);
uint32_t task_get_ttl(task self);
uint8_t task_get_subtasks_amount(task self);
void task_create_subtasks_list(task self, uint8_t subtasks_amount);

// Structures task_struct and task_interface_struct
// are declared here:
#include "task_private.h"

#endif
