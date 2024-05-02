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

// Blocking task
static task_output handle_save(task_input *const input)
{
  if (!list_of_changes_are_there_any_changes())
  {
    return (task_output) {
      .status = EXECUTION_COMPLETED,
      .response = RESPONSE_NONE,
      .request_size = 0,
      .start_next_task_immediately = false
    };
  }


  frame_buffer_status status;

  frame_buffer_reset();
  if (frame_buffer_load_conf())
    END_HANDLE_WITH_ERROR();
  
  uint16_t frames_amount = frame_buffer_get_frames_amount();
  uint16_t current_frame = 0;

  do {
    status = frame_buffer_internal_load();
    if (status)
      END_HANDLE_WITH_ERROR();
    frame_buffer_apply_changes();

    do {
      status = frame_buffer_save();
    //} while (status == FRAME_BUFFER_IN_PROGRESS);
    } while (status);

    // if (status)
    //   END_HANDLE_WITH_ERROR();
  } while (++current_frame < frames_amount);

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