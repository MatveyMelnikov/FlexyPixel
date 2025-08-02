#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "task_cluster.h"
#include "task_manager_defs.h"

task_manager_status task_manager_create(
  task_manager_io io,
  task_cluster *clusters,
  uint32_t clusters_amount
);
void task_manager_destroy(void);
void task_manager_start_cluster(char *const name, task_data *const input);
void task_manager_stop_cluster(char *const name);
void task_manager_execute(void);

#endif
