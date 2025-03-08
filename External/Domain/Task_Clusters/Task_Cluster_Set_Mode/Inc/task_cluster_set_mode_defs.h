#ifndef TASK_CLUSTER_SET_MODE_DEFS_H
#define TASK_CLUSTER_SET_MODE_DEFS_H

#include "task.h"

#define TASK_CLUSTER_SET_MODE_NAME "MODE"

typedef struct
{
  // name, input
  void (*start_cluster)(char *const, task_data *const);
} task_cluster_set_mode_io;

enum
{
  TASK_CLUSTER_SET_MODE_LENGTH = 14U,
  TASK_CLUSTER_SET_MODE_BUFFER_SIZE = TASK_CLUSTER_SET_MODE_LENGTH * 2U,
  TASK_CLUSTER_SET_MODE_SECOND_FIELD_OFFSET = 9U,
};

#endif
