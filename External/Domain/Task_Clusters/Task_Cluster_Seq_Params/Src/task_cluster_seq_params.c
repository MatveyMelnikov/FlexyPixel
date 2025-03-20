#include "task.h"
#include "task_cluster_seq_params.h"
#include "data_transmitter_port.h"
#include "message_handler.h"
#include "mode_repo.h"
#include "displays_config_repo.h"
#include <stdlib.h>
#include <string.h>

// Defines -------------------------------------------------------------------

// Static variables ----------------------------------------------------------

static task_interface_struct interface_request_params;
static task_interface_struct interface_handle_params;
static task_cluster_seq_params_io cluster_io;
static uint8_t input_buffer[TASK_CLUSTER_SEQ_PARAMS_BUFFER_SIZE];

// Static functions ----------------------------------------------------------

static task_output task_request_parameters(task_arg *const argument);
static task_output task_handle_parameters(task_arg *const argument);
static bool task_is_not_seq_parameters(void);
static bool task_set_parameters(void);
static task_output task_handle_error_input(void);

// Implementations -----------------------------------------------------------

task_cluster task_cluster_seq_params_create(task_cluster_seq_params_io io)
{
  cluster_io = io;

  interface_request_params = (task_interface_struct) {
    .run = task_request_parameters,
    .destroy = task_destroy
  };
  interface_handle_params = (task_interface_struct) {
    .run = task_handle_parameters,
    .destroy = task_destroy
  };

  static task cluster_tasks[] = {
    (task) {
      .ttl = 0U,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_request_params
    },
    (task) {
      .ttl = 0U,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_handle_params
    }
  };

  return (task_cluster) {
    .name = TASK_CLUSTER_SEQ_PARAMS_NAME,
    .tasks = cluster_tasks,
    .tasks_amount = (sizeof(cluster_tasks) / sizeof(task))
  };
}

static task_output task_request_parameters(task_arg *const argument)
{
  data_transmitter_port_status status = data_transmitter_port_write(
    (uint8_t*)MESSAGE_HANDLER_RESPONSE_OK,
    strlen(MESSAGE_HANDLER_RESPONSE_OK)
  );
  status |= data_transmitter_port_read(
    input_buffer,
    TASK_CLUSTER_SEQ_PARAMS_LENGTH
  );

  if (status)
    return TASK_OUTPUT_ERROR;

  return (task_output) {
    .response = TASK_RESPONSE_OK,
    .status = TASK_STATUS_COMPLETED
  };
}

static task_output task_handle_parameters(task_arg *const argument)
{
  // {"framesAmount":"030","interframeDelay":"00000000"}
  if (!data_transmitter_port_is_data_received())
    return TASK_OUTPUT_IN_PROGRESS;

  message_handler_set((char*)input_buffer);
  if (task_is_not_seq_parameters())
    return task_handle_error_input();

  if (!task_set_parameters())
    return task_handle_error_input();

  // (void)data_transmitter_port_write(
  //   (uint8_t*)MESSAGE_HANDLER_RESPONSE_OK,
  //   strlen(MESSAGE_HANDLER_RESPONSE_OK)
  // );

  message_handler_unset();
  cluster_io.start_cluster("SEQ_start", NULL);
  return (task_output) {
    .response = TASK_RESPONSE_OK,
    .status = TASK_STATUS_COMPLETED
  };
}

static bool task_is_not_seq_parameters()
{
  if (!message_handler_is_first_field_equal("frame"))
    return true;
  if (
    !message_handler_is_part_equal(
      TASK_CLUSTER_SEQ_PARAMS_SECOND_FIELD_OFFSET,
      "interframeDelay"
    )
  )
  {
    return true;
  }
  return false;
}

static bool task_set_parameters()
{
  char *const frames_amount_str = message_handler_get_part(
    TASK_CLUSTER_SEQ_PARAMS_FRAMES_AMOUNT_OFFSET,
    3U
  );
  uint16_t frames_amount = atoi(frames_amount_str);

  char *const delay_str = message_handler_get_part(
    TASK_CLUSTER_SEQ_PARAMS_DELAY_OFFSET,
    8U
  );
  uint32_t render_delay = atoi(delay_str);

  if (
    frames_amount > TASK_CLUSTER_SEQ_PARAMS_MAX_AMOUNT_OF_FRAMES ||
    render_delay > TASK_CLUSTER_SEQ_PARAMS_MAX_DELAY
  )
  {
    return false;
  }

  const displays_config_repo_external_data *const config =
    displays_config_repo_get();
  displays_config_repo_external_data new_config = {
    .displays_amount = config->displays_amount,
    .frames_amount = frames_amount,
    .render_delay = render_delay
  };
  memcpy(
    new_config.configuration,
    config->configuration,
    sizeof(new_config.configuration)
  );
  (void)displays_config_repo_save(&new_config);

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
