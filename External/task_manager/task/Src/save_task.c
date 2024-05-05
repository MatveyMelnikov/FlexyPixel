#include "save_task.h"
#include "task.h"
#include "debug_output.h"
#include "frame_buffer.h"
#include "list_of_changes.h"
#include <stdlib.h>

// Defines -------------------------------------------------------------------

#define END_HANDLE_WITH_ERROR() \
  { \
    return (task_output) { \
      .status = EXECUTION_ERROR, \
      .response = RESPONSE_ERROR, \
      .request_size = 0U, \
      .start_next_task_immediately = false \
    }; \
  }

// Static variables ----------------------------------------------------------

static task_interface_struct interface;

// Static functions ----------------------------------------------------------

static task_output handle_save(task_input *const input)
{
  if (!list_of_changes_is_need_to_save())
  {
    return (task_output) {
      .status = EXECUTION_COMPLETED,
      .response = RESPONSE_NONE,
      .request_size = 0,
      .start_next_task_immediately = false
    };
  }

  list_of_changes_status status = list_of_changes_save();
  if (status)
  {
    return (task_output) {
      .status = EXECUTION_IN_PROGRESS,
      .response = RESPONSE_NONE,
      .request_size = 0,
      .start_next_task_immediately = false
    };
  }

  return (task_output) {
    .status = EXECUTION_COMPLETED,
    .response = RESPONSE_NONE,
    .request_size = 0,
    .start_next_task_immediately = false
  };
}

static void destroy(task self)
{
  task_destroy(self);
  free(self);
}

// Implementations -----------------------------------------------------------

task save_task_create(void)
{
  task save_task = malloc(sizeof(task_struct));
  save_task->name = "SAVE";
  save_task->vtable = &interface;
  save_task->ttl = 0;

  interface.destroy = destroy;
  task_create_subtasks_list(save_task, 1);
  interface.subtasks[0] = handle_save;

  return save_task;
}