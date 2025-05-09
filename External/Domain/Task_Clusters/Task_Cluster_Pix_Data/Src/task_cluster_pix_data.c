#include "task.h"
#include "task_cluster_pix_data.h"
#include "data_transmitter_port.h"
#include "message_handler.h"
#include "mode_repo.h"
#include "single_changes_repo.h"
#include "debug_handler.h"
#include <stdlib.h>
#include <string.h>

// Defines -------------------------------------------------------------------

// Static variables ----------------------------------------------------------

static task_interface_struct interface_request_data;
static task_interface_struct interface_handle_data;
static task_cluster_pix_data_io cluster_io;
static uint8_t input_buffer[TASK_CLUSTER_PIX_DATA_BUFFER_SIZE];

// Static functions ----------------------------------------------------------

static task_output task_request_data(task_arg *const argument);
static task_output task_handle_data(task_arg *const argument);
static bool task_is_not_data(void);
static task_output task_handle_error_input(void);
static bool task_set_pixel_change(void);
static led_panels_color task_parse_color(void);
static uint8_t task_parse_byte(uint8_t offset);
static uint16_t task_parse_pixel_pos(void);

// Implementations -----------------------------------------------------------

task_cluster task_cluster_pix_data_create(task_cluster_pix_data_io io)
{
  cluster_io = io;

  interface_request_data = (task_interface_struct) {
    .run = task_request_data,
    .destroy = task_destroy
  };
  interface_handle_data = (task_interface_struct) {
    .run = task_handle_data,
    .destroy = task_destroy
  };

  static task cluster_tasks[] = {
    (task) {
      .ttl = 0U,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_request_data
    },
    (task) {
      .ttl = 0U,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_handle_data
    }
  };

  return (task_cluster) {
    .name = TASK_CLUSTER_PIX_DATA_NAME,
    .tasks = cluster_tasks,
    .tasks_amount = (sizeof(cluster_tasks) / sizeof(task))
  };
}

static task_output task_request_data(task_arg *const argument)
{
  // {"panelPosition":9,"pixelColor":"967","pixelPosition":"000"}
  data_transmitter_port_status status = data_transmitter_port_write(
    (uint8_t*)MESSAGE_HANDLER_RESPONSE_OK,
    strlen(MESSAGE_HANDLER_RESPONSE_OK)
  );
  status |= data_transmitter_port_read(
    input_buffer,
    TASK_CLUSTER_PIX_DATA_LENGTH
  );

  if (status)
    return TASK_OUTPUT_ERROR;

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
  if (task_is_not_data())
    return task_handle_error_input();

  DEBUG_HANDLER_FORMAT_OUTPUT("\r\n\tinput data: %s", (char*)input_buffer);

  if (!task_set_pixel_change())
    return task_handle_error_input();

  (void)data_transmitter_port_write(
    (uint8_t*)MESSAGE_HANDLER_RESPONSE_OK,
    strlen(MESSAGE_HANDLER_RESPONSE_OK)
  );
  cluster_io.start_cluster("request_cmd", NULL);
  return TASK_OUTPUT_COMPLETED;
}

static bool task_is_not_data()
{
  if (message_handler_is_first_field_equal("panelPosition"))
    return false;
  if (
    message_handler_is_part_equal(
      TASK_CLUSTER_PIX_DATA_SECOND_FIELD_OFFSET,
      "pixelColor"
    )
  )
  {
    return false;
  }
  if (
    message_handler_is_part_equal(
      TASK_CLUSTER_PIX_DATA_THIRD_FIELD_OFFSET,
      "pixelPosition"
    )
  )
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
  cluster_io.start_cluster("request_cmd", NULL);

  return TASK_OUTPUT_ERROR;
}

static bool task_set_pixel_change()
{
  uint8_t panel_pos = task_parse_byte(TASK_CLUSTER_PIX_DATA_PANEL_POS_OFFSET);
  led_panels_color color = task_parse_color();
  uint16_t pixel_pos = task_parse_pixel_pos();

  if (panel_pos >= SINGLE_CHANGES_REPO_PANELS_AMOUNT)
    return false;
  if (pixel_pos >= LED_PANELS_SIZE_64)
    return false;

  single_changes_repo_set(panel_pos, (uint8_t)pixel_pos, color);
  return true;
}

static led_panels_color task_parse_color()
{  
  return (led_panels_color) {
    .red = task_parse_byte(TASK_CLUSTER_PIX_DATA_PIX_COLOR_OFFSET),
    .green = task_parse_byte(TASK_CLUSTER_PIX_DATA_PIX_COLOR_OFFSET + 1U),
    .blue = task_parse_byte(TASK_CLUSTER_PIX_DATA_PIX_COLOR_OFFSET + 2U)
  };
}

static uint8_t task_parse_byte(uint8_t offset)
{
  char *const byte_str = message_handler_get_part(offset, 1U);
  return atoi(byte_str);
}

static uint16_t task_parse_pixel_pos()
{
  char *const pixel_pos_str = message_handler_get_part(
    TASK_CLUSTER_PIX_DATA_PIX_POS_OFFSET,
    3U
  );
  return atoi(pixel_pos_str);
}
