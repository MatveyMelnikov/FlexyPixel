#include "set_seq_task.h"
#include "hc06_driver.h"
#include "render_controller.h"
#include "render_controller_io.h"
#include "render_controller_defs.h"
#include "led_panels_driver.h"
#include "list_of_changes.h"
#include "displays_conf.h"
#include "debug_output.h"
#include "frame_buffer.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

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

enum {
  FRAME_FIELD_OFFSET = 2U,
  FRAMES_AMOUNT_FIELD_OFFSET = 2U,
  FRAMES_AMOUNT_OFFSET = 17U,
  DELAY_OFFSET = 41U,
  DATA_OFFSET = 10U,
  START_CMD_LENGTH = 51U,
};

static int16_t remaining_frames = 0;
static bool in_progress = false;

// Static variables ----------------------------------------------------------

static task_interface_struct interface;

// Static functions ----------------------------------------------------------

static uint32_t convert_str_to_delay(const uint8_t *const str)
{
  uint32_t result = 0;
  uint32_t delimeter = 10000000;
  for (uint8_t i = 0; i < 8; i++)
  {
    result += (uint32_t)CHAR_TO_NUM(str + i) * delimeter;
    delimeter /= 10;
  }

  return result;
}

static bool is_seq_cmd_wrong(task_input *const input)
{
  return !CHECK_STR(
    input->data + FRAME_FIELD_OFFSET, "frame", strlen("frame")
  );
}

static bool is_amount_cmd_wrong(task_input *const input)
{
  bool is_field_wrong = !CHECK_STR(
    input->data + FRAMES_AMOUNT_FIELD_OFFSET,
    "framesAmount",
    strlen("framesAmount")
  );
  return (displays_conf_is_empty() || is_field_wrong);
}

static bool are_frames_left()
{
  return --remaining_frames > 0;
}

static task_output handle_seq_data(task_input *const input)
{
  // Symbols: 204 (for 8x8 panels)
  // {"frame":"999000777...987"}

  if (!in_progress)
  {
    if (is_seq_cmd_wrong(input))
      END_HANDLE_WITH_ERROR();

    frame_buffer_set(input->data + DATA_OFFSET);
    frame_buffer_save();
    //handler_queue_set_hold(true);
    in_progress = true;
    return (task_output) {
      .status = EXECUTION_IN_PROGRESS,
      .response = RESPONSE_NONE,
      .request_size = 0,
      .start_next_task_immediately = false
    };
  }

  if (frame_buffer_is_busy()) // Page is not saved
  {
    return (task_output) {
      .status = EXECUTION_IN_PROGRESS,
      .response = RESPONSE_NONE,
      .request_size = 0,
      .start_next_task_immediately = false
    };
  }
  if (frame_buffer_save()) // Not all data saved
  {
    return (task_output) {
      .status = EXECUTION_IN_PROGRESS,
      .response = RESPONSE_NONE,
      .request_size = 0,
      .start_next_task_immediately = false
    };
  }

  //handler_queue_set_hold(false);
  in_progress = false;

  if (are_frames_left())
  {
    //handler_queue_skip_remove();
    //hc06_read((uint8_t*)input->data, displays_conf_get_pixels_num() * 3 + 12);
    //render_controller_io_start_timeout_timer();
    //hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));
    return (task_output) {
      .status = EXECUTION_RESTART,
      .response = RESPONSE_OK,
      .request_size = (displays_conf_get_pixels_num() * 3 + 12),
      .start_next_task_immediately = false
    };
  }

  list_of_changes_clear();
  frame_buffer_lock(false);
  //handler_queue_clear();
  //hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  return (task_output) {
    .status = EXECUTION_COMPLETED,
    .response = RESPONSE_OK,
    .request_size = 0,
    .start_next_task_immediately = false
  };
}

static task_output handle_frames_amount(task_input *const input)
{
  // Symbols: 51
  // {"framesAmount":"030","interframeDelay":"00000000"}

  //render_controller_io_stop_timeout_timer();
  if (is_amount_cmd_wrong(input))
    END_HANDLE_WITH_ERROR();

  uint16_t frames_amount = STR_TO_NUM(input->data + FRAMES_AMOUNT_OFFSET);
  if (frames_amount > MAX_FRAMES_AMOUNT)
    END_HANDLE_WITH_ERROR();

  uint32_t delay = convert_str_to_delay(input->data + DELAY_OFFSET);
  
  frame_buffer_reset();
  
  frame_buffer_lock(true);
  frame_buffer_set_render_delay(delay);

  remaining_frames = frames_amount;
  frame_buffer_set_frames_amount(frames_amount);

  //handler_queue_add(handle_seq_data);

  //hc06_read((uint8_t*)input->data, displays_conf_get_pixels_num() * 3 + 12);
  //render_controller_io_start_timeout_timer();

  //hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  return (task_output) {
    .status = EXECUTION_COMPLETED,
    .response = RESPONSE_OK,
    .request_size = (displays_conf_get_pixels_num() * 3 + 12),
    .start_next_task_immediately = false
  };
}

static task_output receive_frames_amount(task_input *const input)
{
  // handler_queue_clear();
  // remaining_frames = 0;

  // handler_queue_add(handle_frames_amount);

  // hc06_read((uint8_t*)input->data, START_CMD_LENGTH);
  // render_controller_io_start_timeout_timer();

  // hc06_write((uint8_t*)OK_STRING, strlen(OK_STRING));
  // DEBUG_OUTPUT("seq mode handler set", strlen("seq mode handler set"));

  remaining_frames = 0;
  
  return (task_output) {
    .status = EXECUTION_COMPLETED,
    .response = RESPONSE_NONE,
    .request_size = START_CMD_LENGTH,
    .start_next_task_immediately = false
  };
}

static void destroy(task self)
{
  task_destroy(self);
  free(self);
}

// Implementations -----------------------------------------------------------

task set_seq_task_create()
{
  task set_seq_task = malloc(sizeof(task_struct));
  set_seq_task->name = "SEQ";
  set_seq_task->vtable = &interface;
  set_seq_task->ttl = 0;

  interface.destroy = destroy;
  task_create_subtasks_list(set_seq_task, 3);
  interface.subtasks[0] = receive_frames_amount;
  interface.subtasks[1] = handle_frames_amount;
  interface.subtasks[2] = handle_seq_data;

  return set_seq_task;
}