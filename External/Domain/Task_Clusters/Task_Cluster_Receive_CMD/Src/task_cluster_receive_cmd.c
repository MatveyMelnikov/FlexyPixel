#include "task.h"
#include "task_cluster_receive_cmd.h"
#include "data_transmitter_port.h"
#include "message_handler.h"
#include <stdlib.h>
#include <string.h>

// Defines -------------------------------------------------------------------

// Static variables ----------------------------------------------------------

static task_interface_struct interface_request_cmd;
static task_interface_struct interface_process_cmd;
static task_cluster_receive_cmd_io cluster_io;
static uint8_t input_buffer[TASK_CLUSTER_RECEIVE_INPUT_BUFFER_SIZE];

// Static functions ----------------------------------------------------------

static task_output task_request_cmd(task_arg *const argument);
static task_output task_process_cmd(task_arg *const argument);
static bool task_is_disconnect_catched(void);
static task_output task_start_save(void);
static task_output task_restart_cluster(void);
static bool task_is_not_data_command(void);
static task_output task_handle_error_input(void);
static char *const task_get_command_type();

// Implementations -----------------------------------------------------------

task_cluster task_cluster_request_cmd_create(task_cluster_receive_cmd_io io)
{
  cluster_io = io;

  interface_request_cmd = (task_interface_struct) {
    .run = task_request_cmd,
    .destroy = task_destroy
  };
  interface_process_cmd = (task_interface_struct) {
    .run = task_process_cmd,
    .destroy = task_destroy
  };

  static task cluster_tasks[] = {
    (task) {
      .ttl = 0U,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_request_cmd
    },
    (task) {
      .ttl = TASK_CLUSTER_RECEIVE_SMD_TTL,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_process_cmd
    }
  };

  return (task_cluster) {
    .name = TASK_CLUSTER_RECEIVE_NAME,
    .tasks = cluster_tasks,
    .tasks_amount = (sizeof(cluster_tasks) / sizeof(task))
  };
}

static task_output task_request_cmd(task_arg *const argument)
{
  data_transmitter_port_status status = data_transmitter_port_read(
    input_buffer,
    TASK_CLUSTER_RECEIVE_SMD_CMD_LEN
  );

  if (status && status != DATA_TRANSMITTER_PORT_STATUS_BUSY)
    return TASK_OUTPUT_ERROR;

  return TASK_OUTPUT_COMPLETED;
}

static task_output task_process_cmd(task_arg *const argument)
{
  message_handler_set((char*)input_buffer);
  if (task_is_disconnect_catched())
    return task_start_save();
  if (!data_transmitter_port_is_data_received())
    return TASK_OUTPUT_IN_PROGRESS;
  
  if (task_is_not_data_command())
    return task_handle_error_input();

  char *command_type = task_get_command_type();
  if (command_type == NULL)
    return task_handle_error_input();
  message_handler_unset();
  
  // (void)data_transmitter_port_write(
  //   (uint8_t*)MESSAGE_HANDLER_RESPONSE_OK,
  //   strlen(MESSAGE_HANDLER_RESPONSE_OK)
  // );

  cluster_io.start_cluster(command_type, NULL);
  return TASK_OUTPUT_COMPLETED;
}

static bool task_is_disconnect_catched()
{
  // +DISC:SUCC
  return message_handler_is_part_equal(
    TASK_CLUSTER_RECEIVE_CMD_DISCONNECT_OFFSET,
    TASK_CLUSTER_RECEIVE_DISCONNECT_CMD
  );
}

static task_output task_start_save(void)
{
  cluster_io.start_cluster("save", NULL);
  return task_restart_cluster();
}

static task_output task_restart_cluster()
{
  cluster_io.start_cluster(TASK_CLUSTER_RECEIVE_NAME, NULL);
  return TASK_OUTPUT_IN_PROGRESS;
}

static bool task_is_not_data_command()
{
  if (message_handler_is_first_field_equal("type"))
  {
    return false;
  }

  return true;
}

static task_output task_handle_error_input()
{
  data_transmitter_port_write(
    (uint8_t*)MESSAGE_HANDLER_RESPONSE_ERROR,
    strlen(MESSAGE_HANDLER_RESPONSE_ERROR)
  );
  message_handler_unset();

  return task_restart_cluster();
}

static char *const task_get_command_type()
{
  return message_handler_get_part(
    TASK_CLUSTER_RECEIVE_CMD_TYPE_OFFSET,
    TASK_CLUSTER_RECEIVE_CMD_TYPE_LEN
  );
}
