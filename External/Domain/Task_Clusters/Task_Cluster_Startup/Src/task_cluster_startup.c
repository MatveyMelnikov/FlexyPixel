#include "task.h"
#include "task_cluster_startup.h"
#include <stdlib.h>
#include <string.h>

// Defines -------------------------------------------------------------------

// Static variables ----------------------------------------------------------

static task_cluster_startuo_io cluster_io;
static task_interface_struct interface_startup;

// Static functions ----------------------------------------------------------

static task_output task_startup(task_arg *const argument);

// Implementations -----------------------------------------------------------

task_cluster task_cluster_startup_create(task_cluster_startuo_io io)
{
  cluster_io = io;

  interface_startup = (task_interface_struct) {
    .run = task_startup,
    .destroy = task_destroy
  };

  static task cluster_tasks[] = {
    (task) {
      .ttl = 0U,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_startup
    }
  };

  return (task_cluster) {
    .name = TASK_CLUSTER_STARTUP_NAME,
    .tasks = cluster_tasks,
    .tasks_amount = (sizeof(cluster_tasks) / sizeof(task))
  };
}

static task_output task_startup(task_arg *const argument)
{
  cluster_io.start_cluster("request_cmd", NULL);
  cluster_io.start_cluster("render", NULL);
  cluster_io.start_cluster("heart_beat", NULL);

  return TASK_OUTPUT_COMPLETED;
}
