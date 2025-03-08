#include "task.h"
#include "task_cluster_seq_data.h"
#include "data_transmitter_port.h"
#include "message_handler.h"
#include "displays_config_repo.h"
#include "frames_repo.h"
#include "led_panels_driver.h"
#include "packed_pixel_data.h"
#include "render_controller.h"
#include <stdlib.h>
#include <string.h>

// Defines -------------------------------------------------------------------

// Static variables ----------------------------------------------------------

static task_interface_struct interface_request_params;
static task_interface_struct interface_handle_params;
static task_cluster_seq_data_io cluster_io;
static uint8_t input_buffer[TASK_CLUSTER_SEQ_DATA_BUFFER_SIZE];
static uint8_t pixel_data[FRAMES_REPO_FRAME_DATA_SIZE];
static uint16_t current_frame = 0U;

// Static functions ----------------------------------------------------------

static task_output task_request_data(task_arg *const argument);
static task_output task_handle_data(task_arg *const argument);
static bool task_is_not_frame_data(void);
static task_output task_handle_error_input(void);
static bool task_save_frame(void);
static task_output task_restart_cluster(void);

// Implementations -----------------------------------------------------------

task_cluster task_cluster_seq_data_create(task_cluster_seq_data_io io)
{
  cluster_io = io;

  interface_request_params = (task_interface_struct) {
    .run = task_request_data,
    .destroy = task_destroy
  };
  interface_handle_params = (task_interface_struct) {
    .run = task_handle_data,
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
    .name = TASK_CLUSTER_SEQ_DATA_NAME,
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
  uint16_t data_frame_size = 10U + 
    displays_config_repo_get()->displays_amount * 192U + 2U;
  status |= data_transmitter_port_read(
    input_buffer,
    data_frame_size
  );

  if (status)
    return TASK_OUTPUT_ERROR;
  render_controller_pause(true);

  return (task_output) {
    .response = TASK_RESPONSE_OK,
    .status = TASK_STATUS_COMPLETED
  };
}

static task_output task_handle_data(task_arg *const argument)
{
  if (!data_transmitter_port_is_data_received())
    return TASK_OUTPUT_IN_PROGRESS;

  message_handler_set((char*)input_buffer);
  if (task_is_not_frame_data())
    return task_handle_error_input();
  
  if (!task_save_frame())
    return task_handle_error_input();

  if (++current_frame >= (displays_config_repo_get()->frames_amount))
  {
    current_frame = 0U;

    (void)data_transmitter_port_write(
      (uint8_t*)MESSAGE_HANDLER_RESPONSE_OK,
      strlen(MESSAGE_HANDLER_RESPONSE_OK)
    );
    cluster_io.start_cluster("request_cmd", NULL);
    render_controller_reset();
    render_controller_update_config();
    return TASK_OUTPUT_COMPLETED;
  }
  return task_restart_cluster(); 
}

static bool task_is_not_frame_data(void)
{
  return !message_handler_is_first_field_equal("frame");
}

static task_output task_handle_error_input()
{
  data_transmitter_port_write(
    (uint8_t*)MESSAGE_HANDLER_RESPONSE_ERROR,
    strlen(MESSAGE_HANDLER_RESPONSE_ERROR)
  );
  message_handler_unset();
  cluster_io.start_cluster("request_cmd", NULL);
  current_frame = 0U;

  return TASK_OUTPUT_ERROR;
}

static bool task_save_frame()
{
  uint8_t displays_amount = displays_config_repo_get()->displays_amount;
  // We can use this function with strings, since the numbers to be extracted
  // are in the second half of the byte (according to the Ascii table):
  // '0' - 0011.0000, '1' - 0011.0001, '2' - 0011.0010, ...
  packed_pixel_data_pack(
    (const packed_pixel_data_color*)(input_buffer +
      TASK_CLUSTER_SEQ_DATA_FRAME_DATA_OFFSET),
    pixel_data,
    displays_amount
  );

  frames_repo_status status = frames_repo_save(
    current_frame,
    pixel_data,
    displays_amount * 96U
  );

  return (status == FRAMES_REPO_OK);
}

static task_output task_restart_cluster(void)
{
  cluster_io.start_cluster(TASK_CLUSTER_SEQ_DATA_NAME, NULL);
  return TASK_OUTPUT_IN_PROGRESS;
}
