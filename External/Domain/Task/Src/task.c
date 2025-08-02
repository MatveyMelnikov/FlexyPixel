#include "task.h"
#include "task_defs.h"
#include <stddef.h>
#include <stdlib.h>

// Static prototypes ---------------------------------------------------------

static bool task_is_ttl_ends(task_ptr self, uint32_t current_tick);

// Implementations -----------------------------------------------------------

void task_destroy(task_ptr self)
{
  self->vtable->destroy(self);
}

task_output task_execute(
  task_ptr self,
  task_arg *const argument
)
{
  if (task_is_ttl_ends(self, argument->current_tick))
    return TASK_OUTPUT_COMPLETED;

  return self->vtable->run(argument);
}

static bool task_is_ttl_ends(task_ptr self, uint32_t current_tick)
{
  if (self->is_infinite)
    return false;
  if ((current_tick - self->start_tick) > self->ttl)
    return true;
  
  return false;
}

uint32_t task_get_ttl(task_ptr self)
{
  return self->ttl;
}

uint32_t task_get_start_tick(task_ptr self)
{
  return self->start_tick;
}

void task_set_start_tick(task_ptr self, uint32_t tick)
{
  self->start_tick = tick;
}

bool task_is_infinite(task_ptr self)
{
  return self->is_infinite;
}
