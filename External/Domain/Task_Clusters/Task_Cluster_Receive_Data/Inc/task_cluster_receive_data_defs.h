#ifndef TASK_CLUSTER_RECEIVE_DATA_DEFS_H
#define TASK_CLUSTER_RECEIVE_DATA_DEFS_H

#include "task.h"

#define TASK_CLUSTER_RECEIVE_DATA_NAME "DATA"

typedef struct
{
  // name, input
  void (*start_cluster)(char *const, task_data *const);
} task_cluster_receive_data_io;

#endif
