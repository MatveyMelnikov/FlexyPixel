#include "set_mode_task.h"
#include "task_input.h"
#include "task_output.h"
#include "render_controller_io.h"
#include "render_controller_defs.h"
#include "displays_conf.h"
#include "task_manager.h"
#include <stdlib.h>

// Typedef -------------------------------------------------------------------

enum {
  MODE_CMD_LENGTH = 14U,
};

// Static variables ----------------------------------------------------------

static task_interface_struct interface;

// Static functions ----------------------------------------------------------

static task_output handle_mode(task_input *const input)
{
  if (is_first_field_not_suitable(input->data, "mode"))
  {
    return (task_output) {
      .status = EXECUTION_ERROR,
      .response = RESPONSE_NONE,
      .request_size = 0,
      .start_next_task_immediately = false
    };
  }

  bool success = task_manager_set_data_task((char*)input->data + MODE_OFFSET);

  return (task_output) {
    .status = success ? EXECUTION_COMPLETED : EXECUTION_ERROR,
    .response = success ? RESPONSE_OK : RESPONSE_ERROR,
    .request_size = 0U,
    .start_next_task_immediately = false
  };
}

static task_output set_handler(task_input *const input)
{
  return (task_output) {
    .status = EXECUTION_COMPLETED,
    .response = RESPONSE_OK,
    .request_size = MODE_CMD_LENGTH,
    .start_next_task_immediately = false
  };
}

static void destroy(task self)
{
  task_destroy(self);
  free(self);
}

// Implementations -----------------------------------------------------------

task set_mode_task_create()
{
  task set_mode_task = malloc(sizeof(task_struct));
  set_mode_task->name = "MODE";
  set_mode_task->vtable = &interface;
  set_mode_task->ttl = 0;

  interface.destroy = destroy;
  task_create_subtasks_list(set_mode_task, 2);
  interface.subtasks[0] = set_handler;
  interface.subtasks[1] = handle_mode;

  return set_mode_task;
}
