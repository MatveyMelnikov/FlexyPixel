#include "task.h"
#include "task_cluster_reset.h"
#include "single_changes_repo.h"
#include "displays_config_repo.h"
#include "message_handler.h"
#include "data_transmitter_port.h"
#include <string.h>
#include <stdlib.h>

// Defines -------------------------------------------------------------------

// Static variables ----------------------------------------------------------

static task_interface_struct interface_save;
static task_cluster_reset_io cluster_io;

// Static functions ----------------------------------------------------------

static task_output task_reset(task_arg *const argument);

// Implementations -----------------------------------------------------------

task_cluster task_cluster_reset_create(task_cluster_reset_io io)
{
  cluster_io = io;

  interface_save = (task_interface_struct) {
    .run = task_reset,
    .destroy = task_destroy
  };

  static task cluster_tasks[] = {
    (task) {
      .ttl = TASK_CLUSTER_RESET_TTL,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_save
    }
  };

  return (task_cluster) {
    .name = TASK_CLUSTER_RESET_NAME,
    .tasks = cluster_tasks,
    .tasks_amount = (sizeof(cluster_tasks) / sizeof(task))
  };
}

static task_output task_reset(task_arg *const argument)
{
  single_changes_repo_reset();
  single_changes_repo_save(); // save empty changes

  displays_config_repo_external_data new_config = { 0 };
  // save empty config
  displays_config_repo_save(&new_config);

  (void)data_transmitter_port_write(
    (uint8_t*)MESSAGE_HANDLER_RESPONSE_OK,
    strlen(MESSAGE_HANDLER_RESPONSE_OK)
  );

  cluster_io.start_cluster("request_cmd", NULL);
  return TASK_OUTPUT_COMPLETED;
}
