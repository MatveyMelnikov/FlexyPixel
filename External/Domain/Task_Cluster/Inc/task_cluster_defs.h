#ifndef TASK_CLUSTER_DEFS_H
#define TASK_CLUSTER_DEFS_H

#include "task.h"

typedef enum
{
  TASK_CLUSTER_STATUS_COMPLETED = 0U,
  TASK_CLUSTER_STATUS_IN_PROGRESS
} task_cluster_status;

typedef struct
{
  char *name;
  task *tasks;
  uint32_t tasks_amount;
  uint32_t task_index;
  task_cluster_status status;
  task_data *input;
  task_data *output;
} task_cluster;

#endif
