#include "task.h"
#include "task_cluster_set_mode.h"
#include "data_transmitter_port.h"
#include "message_handler.h"
#include "mode_repo.h"
#include "debug_handler.h"
#include <stdlib.h>
#include <string.h>

// Defines -------------------------------------------------------------------

// Static variables ----------------------------------------------------------

static task_interface_struct interface_request_mode;
static task_interface_struct interface_set_mode;
static task_cluster_set_mode_io cluster_io;
static uint8_t input_buffer[TASK_CLUSTER_SET_MODE_BUFFER_SIZE];

// Static functions ----------------------------------------------------------

static task_output task_request_mode(task_arg *const argument);
static task_output task_set_mode(task_arg *const argument);
static bool task_is_not_mode(void);
static task_output task_handle_error_input(void);
static bool task_handle_set_mode(void);

// Implementations -----------------------------------------------------------

task_cluster task_cluster_set_mode_create(task_cluster_set_mode_io io)
{
  cluster_io = io;

  interface_request_mode = (task_interface_struct) {
    .run = task_request_mode,
    .destroy = task_destroy
  };
  interface_set_mode = (task_interface_struct) {
    .run = task_set_mode,
    .destroy = task_destroy
  };

  static task cluster_tasks[] = {
    (task) {
      .ttl = 0U,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_request_mode
    },
    (task) {
      .ttl = 0U,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_set_mode
    }
  };

  return (task_cluster) {
    .name = TASK_CLUSTER_SET_MODE_NAME,
    .tasks = cluster_tasks,
    .tasks_amount = (sizeof(cluster_tasks) / sizeof(task))
  };
}

static task_output task_request_mode(task_arg *const argument)
{
  // {"mode":"SEQ"} ("PIX")
  data_transmitter_port_status status = data_transmitter_port_write(
    (uint8_t*)MESSAGE_HANDLER_RESPONSE_OK,
    strlen(MESSAGE_HANDLER_RESPONSE_OK)
  );
  status |= data_transmitter_port_read(
    input_buffer,
    TASK_CLUSTER_SET_MODE_LENGTH
  );

  if (status)
    return TASK_OUTPUT_ERROR;

  return (task_output) {
    .response = TASK_RESPONSE_OK,
    .status = TASK_STATUS_COMPLETED
  };
}

static task_output task_set_mode(task_arg *const argument)
{
  if (!data_transmitter_port_is_data_received())
    return TASK_OUTPUT_IN_PROGRESS;

  message_handler_set((char*)input_buffer);
  if (task_is_not_mode())
    return task_handle_error_input();

  DEBUG_HANDLER_FORMAT_OUTPUT("\r\n\tnew mode: %s", (char*)input_buffer);

  if (!task_handle_set_mode())
    return task_handle_error_input();

  (void)data_transmitter_port_write(
    (uint8_t*)MESSAGE_HANDLER_RESPONSE_OK,
    strlen(MESSAGE_HANDLER_RESPONSE_OK)
  );
  cluster_io.start_cluster("request_cmd", NULL);
  return TASK_OUTPUT_COMPLETED;
}

static bool task_is_not_mode()
{
  if (message_handler_is_first_field_equal("mode"))
    return false;

  return true;
}

static task_output task_handle_error_input()
{
  data_transmitter_port_write(
    (uint8_t*)MESSAGE_HANDLER_RESPONSE_ERROR,
    strlen(MESSAGE_HANDLER_RESPONSE_ERROR)
  );
  message_handler_unset();
  cluster_io.start_cluster("request_cmd", NULL);

  return TASK_OUTPUT_ERROR;
}

static bool task_handle_set_mode()
{
  char *mode = message_handler_get_part(
    TASK_CLUSTER_SET_MODE_SECOND_FIELD_OFFSET,
    3U
  );

  return mode_repo_set(mode);
}
