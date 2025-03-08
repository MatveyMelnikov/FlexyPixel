#ifndef TASK_CLUSTER_SAVE_DEFS_H
#define TASK_CLUSTER_SAVE_DEFS_H

#include "task.h"

#define TASK_CLUSTER_SAVE_NAME "save"

typedef struct
{
  // name, input
  void (*start_cluster)(char *const, task_data *const);
} task_cluster_save_io;

enum
{
  TASK_CLUSTER_SAVE_TTL = 100U
};

#endif
