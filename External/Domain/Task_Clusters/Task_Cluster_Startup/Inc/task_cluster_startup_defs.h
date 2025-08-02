#ifndef TASK_CLUSTER_STARTUP_DEFS_H
#define TASK_CLUSTER_STARTUP_DEFS_H

#define TASK_CLUSTER_STARTUP_NAME "startup"

typedef struct
{
  // name, input
  void (*start_cluster)(char *const, task_data *const);
} task_cluster_startuo_io;

#endif
