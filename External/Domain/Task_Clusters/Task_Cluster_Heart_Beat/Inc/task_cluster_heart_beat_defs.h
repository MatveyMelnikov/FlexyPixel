#ifndef TASK_CLUSTER_HEART_BEAT_DEFS_H
#define TASK_CLUSTER_HEART_BEAT_DEFS_H

#define TASK_CLUSTER_HEART_BEAT_NAME "heart_beat"

enum {
  TASK_CLUSTER_HEART_BEAT_DELAY = 500U
};

typedef struct
{
  void (*toggle_led)(void);
} task_cluster_heart_beat_io;

#endif
