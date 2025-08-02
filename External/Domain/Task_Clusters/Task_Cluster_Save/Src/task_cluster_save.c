#include "task.h"
#include "task_cluster_save.h"
#include "single_changes_repo.h"
#include "message_handler.h"
#include "data_transmitter_port.h"
#include <string.h>
#include <stdlib.h>

// Defines -------------------------------------------------------------------

// Static variables ----------------------------------------------------------

static task_interface_struct interface_save;
static task_cluster_save_io cluster_io;

// Static functions ----------------------------------------------------------

static task_output task_save(task_arg *const argument);

// Implementations -----------------------------------------------------------

task_cluster task_cluster_save_create(task_cluster_save_io io)
{
  cluster_io = io;

  interface_save = (task_interface_struct) {
    .run = task_save,
    .destroy = task_destroy
  };

  static task cluster_tasks[] = {
    (task) {
      .ttl = TASK_CLUSTER_SAVE_TTL,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_save
    }
  };

  return (task_cluster) {
    .name = TASK_CLUSTER_SAVE_NAME,
    .tasks = cluster_tasks,
    .tasks_amount = (sizeof(cluster_tasks) / sizeof(task))
  };
}

static task_output task_save(task_arg *const argument)
{
  // Configuration and frames actually saved
  single_changes_repo_save();

  (void)data_transmitter_port_write(
    (uint8_t*)MESSAGE_HANDLER_RESPONSE_OK,
    strlen(MESSAGE_HANDLER_RESPONSE_OK)
  );

  cluster_io.start_cluster("request_cmd", NULL);
  return TASK_OUTPUT_COMPLETED;
}
