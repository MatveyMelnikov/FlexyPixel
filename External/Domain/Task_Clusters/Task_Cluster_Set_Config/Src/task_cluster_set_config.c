#include "task.h"
#include "task_cluster_set_config.h"
#include "displays_config_repo.h"
#include "data_transmitter_port.h"
#include "message_handler.h"
#include "render_controller.h"
#include "frames_repo.h"
#include "single_changes_repo.h"
#include <string.h>
#include <stdlib.h>

// Defines -------------------------------------------------------------------

// Static variables ----------------------------------------------------------

static task_interface_struct interface_request_data;
static task_interface_struct interface_set_config;
static task_cluster_set_config_io cluster_io;
static uint8_t input_buffer[TASK_CLUSTER_SET_CONFIG_CMD_LENGTH];

// Static functions ----------------------------------------------------------

static task_output task_request_data(task_arg *const argument);
static task_output task_set_config(task_arg *const argument);
static bool task_is_not_config(void);
static task_output task_handle_error_input(void);
static bool task_parse_config(
  displays_config_repo_external_data *const result
);
static bool task_is_invalid_display_size(uint16_t size);
static void task_reset_frames(void);

// Implementations -----------------------------------------------------------

task_cluster task_cluster_set_config_create(task_cluster_set_config_io io)
{
  cluster_io = io;

  interface_request_data = (task_interface_struct) {
    .run = task_request_data,
    .destroy = task_destroy
  };
  interface_set_config = (task_interface_struct) {
    .run = task_set_config,
    .destroy = task_destroy
  };

  static task cluster_tasks[] = {
    (task) {
      .ttl = TASK_CLUSTER_REQUEST_DATA_TTL,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_request_data
    },
    (task) {
      .ttl = TASK_CLUSTER_SET_CONFIG_TTL,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_set_config
    }
  };

  return (task_cluster) {
    .name = TASK_CLUSTER_SET_CONFIG_NAME,
    .tasks = cluster_tasks,
    .tasks_amount = (sizeof(cluster_tasks) / sizeof(task))
  };
}

static task_output task_request_data(task_arg *const argument)
{
  data_transmitter_port_status status = data_transmitter_port_write(
    (uint8_t*)MESSAGE_HANDLER_RESPONSE_OK,
    strlen(MESSAGE_HANDLER_RESPONSE_OK)
  );
  status |= data_transmitter_port_read(
    input_buffer,
    TASK_CLUSTER_SET_CONFIG_CMD_LENGTH
  );

  if (status)
    return TASK_OUTPUT_ERROR;

  return (task_output) {
    .response = TASK_RESPONSE_OK,
    .status = TASK_STATUS_COMPLETED
  };
}

static task_output task_set_config(task_arg *const argument)
{
  if (!data_transmitter_port_is_data_received())
    return TASK_OUTPUT_IN_PROGRESS;

  message_handler_set((char*)input_buffer);
  if (task_is_not_config())
    return task_handle_error_input();

  displays_config_repo_external_data new_config = { 0 };
  if (!task_parse_config(&new_config))
    return task_handle_error_input();

  task_reset_frames();
  displays_config_repo_status status = displays_config_repo_save(&new_config);
  if (status)
    return task_handle_error_input();
  
  single_changes_repo_reset();
  single_changes_repo_save();
  render_controller_update_config();
  
  (void)data_transmitter_port_write(
    (uint8_t*)MESSAGE_HANDLER_RESPONSE_OK,
    strlen(MESSAGE_HANDLER_RESPONSE_OK)
  );
  cluster_io.start_cluster("request_cmd", NULL);
  return TASK_OUTPUT_COMPLETED;
}

static bool task_is_not_config()
{
  if (message_handler_is_first_field_equal("configuration"))
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

static bool task_parse_config(
  displays_config_repo_external_data *const result
)
{
  // {"configuration":"256064000000000000000000000"}
  uint16_t size = 0U;

  for (
    uint8_t display_index = 0;
    display_index < DISPLAYS_CONFIG_REPO_DISPLAYS_MAX_AMOUNT;
    display_index++
  )
  {
    uint8_t offset = TASK_CLUSTER_SET_CONFIG_OFFSET + (3U * display_index);
    char *const string_number = message_handler_get_part(offset, 3U);

    size = (uint16_t)atoi(string_number);
    if (size == 0U)
      break;
    if (task_is_invalid_display_size(size))
      return false;

    result->configuration[display_index] = (displays_config_repo_size)size;
    result->displays_amount++;
  }
  result->frames_amount = 1U;

  return true;
}

static bool task_is_invalid_display_size(uint16_t size)
{
  return (
    size != DISPLAYS_CONFIG_REPO_SMALL &&
    size != DISPLAYS_CONFIG_REPO_BIG
  );
}

static void task_reset_frames(void)
{
  for (
    uint8_t frame_index = 0;
    frame_index < displays_config_repo_get()->frames_amount;
    frame_index++
  )
  {
    frames_repo_reset(frame_index);
  }
}
