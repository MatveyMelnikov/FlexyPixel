#include "task_cluster.h"
#include <stddef.h>

// Static prototypes ---------------------------------------------------------

static void task_cluster_set_start_tick_to_task(
  task_cluster *const self,
  uint32_t index,
  uint32_t tick
);
static bool task_cluster_are_all_tasks_completed(task_cluster *const self);
static void task_cluster_increment_task_index(
  task_cluster *const self,
  task_output output,
  uint32_t current_tick
);

// Implementations -----------------------------------------------------------

void task_cluster_create(
  task_cluster *const self,
  char *name,
  task *const tasks,
  uint32_t tasks_amount
)
{
  self->name = name;
  self->tasks = tasks;
  self->tasks_amount = tasks_amount;
  self->task_index = 0U;
  self->status = TASK_CLUSTER_STATUS_COMPLETED;
}

void task_cluster_destroy(task_cluster *const self)
{
  self->name = NULL;
  self->tasks = NULL;
  self->tasks = 0U;
  self->task_index = 0U;
  self->status = TASK_CLUSTER_STATUS_COMPLETED;
}

void task_cluster_start(
  task_cluster *const self,
  task_arg argument
)
{
  self->task_index = 0;
  self->status = TASK_CLUSTER_STATUS_IN_PROGRESS;
  self->input = argument.input;
  self->output = argument.output;

  task_cluster_set_start_tick_to_task(self, 0U, argument.current_tick);
}

static void task_cluster_set_start_tick_to_task(
  task_cluster *const self,
  uint32_t index,
  uint32_t tick
)
{
  if (index >= self->tasks_amount)
    return;

  task_set_start_tick(&self->tasks[index], tick);
}

void task_cluster_stop(task_cluster *const self)
{
  self->status = TASK_CLUSTER_STATUS_COMPLETED;
}

task_output task_cluster_execute(
  task_cluster *const self,
  uint32_t current_tick
)
{
  task_output task_out = { 0 };
  task_arg argument = (task_arg) {
    .input = self->input,
    .output = self->output,
    .current_tick = current_tick
  };

  if (self->status == TASK_CLUSTER_STATUS_COMPLETED)
    return TASK_OUTPUT_COMPLETED;

  task_out = task_execute(
    &self->tasks[self->task_index],
    &argument
  );

  task_cluster_increment_task_index(self, task_out, current_tick);

  return task_out;
}

static bool task_cluster_are_all_tasks_completed(task_cluster *const self)
{
  return (self->task_index >= self->tasks_amount);
}

static void task_cluster_increment_task_index(
  task_cluster *const self,
  task_output output,
  uint32_t current_tick
)
{
  if (output.status == TASK_STATUS_COMPLETED)
  {
    self->task_index++;
    task_cluster_set_start_tick_to_task(self, self->task_index, current_tick);
  }

  if (task_cluster_are_all_tasks_completed(self))
    self->status = TASK_CLUSTER_STATUS_COMPLETED;
}
