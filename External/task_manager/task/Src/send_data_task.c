#include "send_data_task.h"
#include "task_manager.h"
#include "render_controller_io.h"
#include "render_controller_defs.h"
#include "displays_conf.h"
#include "debug_output.h"
#include <stdlib.h>

// Static variables ----------------------------------------------------------

static task_interface_struct interface;

// Static functions ----------------------------------------------------------

static task_output set_tasks(task_input *const input)
{
  task data_task = task_manager_get_data_task();

  if (data_task == NULL || displays_conf_is_empty())
  {
    return (task_output) {
      .status = EXECUTION_ERROR,
      .response = RESPONSE_UNCONFIGURED,
      .request_size = 0U,
      .start_next_task_immediately = false
    };
  }

  DEBUG_OUTPUT("data handler set", strlen("data handler set"));

  bool success = task_manager_set(data_task->name);

  return (task_output) {
    .status = success ? EXECUTION_COMPLETED : EXECUTION_ERROR,
    .response = success ? RESPONSE_OK : RESPONSE_ERROR,
    .request_size = 0U,
    .start_next_task_immediately = true
  };
}

static void destroy(task self)
{
  task_destroy(self);
  free(self);
}

// Implementations -----------------------------------------------------------

task send_data_task_create()
{
  task send_data_task = malloc(sizeof(task_struct));
  send_data_task->name = "DATA";
  send_data_task->vtable = &interface;
  send_data_task->ttl = 0;

  interface.destroy = destroy;
  task_create_subtasks_list(send_data_task, 1);
  interface.subtasks[0] = set_tasks;

  return send_data_task;
}
