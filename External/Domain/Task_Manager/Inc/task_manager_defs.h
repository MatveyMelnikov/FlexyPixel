#ifndef TASK_MANAGER_DEFS_H
#define TASK_MANAGER_DEFS_H

#include "task_cluster.h"

typedef struct
{
  uint32_t (*get_tick)(void);
} task_manager_io;

typedef enum
{
  TASK_MANAGER_STATUS_OK = 0U,
  TASK_MANAGER_STATUS_ERROR
} task_manager_status;

enum {
  TASK_MANAGER_CLUSTER_MAX_AMOUNT = 15U,
  TASK_MANAGER_SHARED_BUFFER_SIZE = 1024U,
};

#endif
