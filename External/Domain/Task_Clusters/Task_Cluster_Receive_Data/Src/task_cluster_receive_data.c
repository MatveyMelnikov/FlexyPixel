#include "task.h"
#include "task_cluster_receive_data.h"
#include "message_handler.h"
#include "mode_repo.h"
#include "displays_config_repo.h"
#include "data_transmitter_port.h"
#include "debug_handler.h"
#include <stdlib.h>
#include <string.h>

// Defines -------------------------------------------------------------------

// Static variables ----------------------------------------------------------

static task_interface_struct interface_start_data_task;
static task_cluster_receive_data_io cluster_io;

// Static functions ----------------------------------------------------------

static task_output task_start_data_task(task_arg *const argument);
static task_output task_handle_unconfigure_input(void);

// Implementations -----------------------------------------------------------

task_cluster task_cluster_receive_data_create(
  task_cluster_receive_data_io io
)
{
  cluster_io = io;

  interface_start_data_task = (task_interface_struct) {
    .run = task_start_data_task,
    .destroy = task_destroy
  };

  static task cluster_tasks[] = {
    (task) {
      .ttl = 0U,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_start_data_task
    }
  };

  return (task_cluster) {
    .name = TASK_CLUSTER_RECEIVE_DATA_NAME,
    .tasks = cluster_tasks,
    .tasks_amount = (sizeof(cluster_tasks) / sizeof(task))
  };
}

static task_output task_start_data_task(task_arg *const argument)
{
  // We check whether the operating mode and display configuration are set
  if (!mode_is_mode_set())
    task_handle_unconfigure_input();
  
  const displays_config_repo_external_data *current_config = 
    displays_config_repo_get();
  if (current_config->displays_amount == 0)
    task_handle_unconfigure_input();

  DEBUG_HANDLER_OUTPUT("\r\n\tconfig and mode is OK");

  // Start data cluster according to the operating mode (SEQ, PIX, ...)
  cluster_io.start_cluster((char*)mode_repo_get(), NULL);
  return TASK_OUTPUT_COMPLETED;
}

static task_output task_handle_unconfigure_input()
{
  (void)data_transmitter_port_write(
    (uint8_t*)MESSAGE_HANDLER_RESPONSE_UNCONFIGURED,
    strlen(MESSAGE_HANDLER_RESPONSE_UNCONFIGURED)
  );
  message_handler_unset();
  cluster_io.start_cluster("request_cmd", NULL);

  return (task_output) {
    .response = TASK_RESPONSE_UNCONFIGURED,
    .status = TASK_STATUS_COMPLETED
  };
}
