#include "task.h"
#include <stddef.h>
#include <stdlib.h>

// Implementations -----------------------------------------------------------

void task_destroy(task self)
{
  self->vtable->destroy(self);
}

task_output task_execute_subtask(
  task self,
  task_input *const input,
  uint8_t subtask_index
)
{
  //self->vtable->set_handlers(self, input);
  return self->vtable->subtasks[subtask_index](input);
}

char *const task_get_name(task self)
{
  return self->name;
}

uint32_t task_get_ttl(task self)
{
  return self->ttl;
}

uint8_t task_get_subtasks_amount(task self)
{
  return self->subtasks_amount;
}

void task_create_subtasks_list(task self, uint8_t subtasks_amount)
{
  self->subtasks_amount = subtasks_amount;
  self->vtable->subtasks = malloc(sizeof(void*) * subtasks_amount);
}
