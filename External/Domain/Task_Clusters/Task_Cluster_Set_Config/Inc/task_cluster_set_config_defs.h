#ifndef TASK_CLUSTER_SET_CONFIG_DEFS_H
#define TASK_CLUSTER_SET_CONFIG_DEFS_H

#include "task.h"

#define TASK_CLUSTER_SET_CONFIG_NAME "CONF"

typedef struct
{
  // name, input
  void (*start_cluster)(char *const, task_data *const);
} task_cluster_set_config_io;

enum
{
  TASK_CLUSTER_SET_CONFIG_FIRST_FIELD_OFFSET = 2U,
  TASK_CLUSTER_SET_CONFIG_OFFSET = 18U,
  TASK_CLUSTER_SET_CONFIG_CMD_LENGTH = 47U,
  TASK_CLUSTER_REQUEST_DATA_TTL = 100U,
  TASK_CLUSTER_SET_CONFIG_TTL = 100U
};

#endif
