#ifndef TASK_CLUSTER_SEQ_DATA_DEFS_H
#define TASK_CLUSTER_SEQ_DATA_DEFS_H

#include "task.h"

#define TASK_CLUSTER_SEQ_DATA_NAME "SEQ_start"

typedef struct
{
  // name, input
  void (*start_cluster)(char *const, task_data *const);
} task_cluster_seq_data_io;

enum
{
  TASK_CLUSTER_SEQ_DATA_BUFFER_SIZE = 1800U,
  TASK_CLUSTER_SEQ_DATA_FRAME_DATA_OFFSET = 10U,
};

#endif
