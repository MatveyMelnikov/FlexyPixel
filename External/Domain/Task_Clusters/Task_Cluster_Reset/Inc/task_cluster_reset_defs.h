#ifndef TASK_CLUSTER_RESET_DEFS_H
#define TASK_CLUSTER_RESET_DEFS_H

#include "task.h"

#define TASK_CLUSTER_RESET_NAME "RSTP"

typedef struct
{
  // name, input
  void (*start_cluster)(char *const, task_data *const);
} task_cluster_reset_io;

enum
{
  TASK_CLUSTER_RESET_TTL = 100U
};

#endif
