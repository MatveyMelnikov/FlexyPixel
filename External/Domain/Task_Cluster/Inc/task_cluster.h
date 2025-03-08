#ifndef TASK_CLUSTER_H
#define TASK_CLUSTER_H

#include "task_cluster_defs.h"
#include "task.h"

void task_cluster_create(
  task_cluster *const self,
  char *name,
  task *const tasks,
  uint32_t tasks_amount
);
void task_cluster_destroy(task_cluster *const self);
void task_cluster_start(
  task_cluster *const self,
  task_arg argument
);
void task_cluster_stop(task_cluster *const self);
task_output task_cluster_execute(
  task_cluster *const self,
  uint32_t current_tick
);

#endif
