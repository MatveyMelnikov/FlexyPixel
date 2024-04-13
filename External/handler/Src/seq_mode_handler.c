#include "seq_mode_handler.h"
#include "hc06_driver.h"
#include "render_controller.h"
#include "render_controller_io.h"
#include "render_controller_defs.h"
#include "handler_queue.h"
#include "led_panels_driver.h"
#include "list_of_changes.h"
#include "displays_conf.h"
#include "debug_output.h"
#include "frame_buffer.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
// #include "stm32f1xx.h"

// Defines -------------------------------------------------------------------

#define END_HANDLE_WITH_ERROR() \
  { \
    hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING)); \
    return; \
  }

// #define STR_TO_DELAY(str) \
//   (uint16_t)CHAR_TO_NUM(str) * 10000000 + \
//   (uint16_t)CHAR_TO_NUM(str + 1) * 1000000 + \
//   (uint16_t)CHAR_TO_NUM(str + 2) * 100000 + \
//   (uint16_t)CHAR_TO_NUM(str + 3) * 10000 + \
//   (uint16_t)CHAR_TO_NUM(str + 4) * 1000 + \
//   (uint16_t)CHAR_TO_NUM(str + 5) * 100 + \
//   (uint16_t)CHAR_TO_NUM(str + 6) * 10 + \
//   (uint16_t)CHAR_TO_NUM(str + 7)

// Static variables ----------------------------------------------------------

static int16_t remaining_frames = 0;

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

static bool is_seq_cmd_wrong(handler_input *const input)
{
  return !CHECK_STR(input->data + 2, "frame", 5);
}

static bool is_amount_cmd_wrong(handler_input *const input)
{
  bool is_field_wrong = !CHECK_STR(input->data + 2, "framesAmount", 12);
  return (displays_conf_is_empty() || is_field_wrong);
}

static bool are_frames_left()
{
  return --remaining_frames > 0;
}

static void handle_seq_data(handler_input *const input)
{
  // Symbols: 204 (for 8x8 panels)
  // {"frame":"999000777...987"}

  render_controller_io_stop_timeout_timer();

  if (!handler_queue_get_hold_flag()) // First call
  {
    if (is_seq_cmd_wrong(input))
      END_HANDLE_WITH_ERROR();

    frame_buffer_lock(true);
    frame_buffer_set(input->data + 10);
    frame_buffer_save();
    handler_queue_set_hold(true);
    return;
  }

  if (frame_buffer_is_busy()) // Page is not saved
    return;
  if (frame_buffer_save()) // Not all data saved
    return;

  handler_queue_set_hold(false);

  if (are_frames_left())
  {
    handler_queue_skip_remove();
    hc06_read((uint8_t*)input->data, displays_conf_get_pixels_num() * 3 + 12);
    render_controller_io_start_timeout_timer();
    hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));
    return;
  }

  frame_buffer_lock(false);
  handler_queue_clear();
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));
}

static void handle_frames_amount(handler_input *const input)
{
  // Symbols: 23
  // {"framesAmount":"030"}
  
  
  // Symbols: 51
  // {"framesAmount":"030","interframeDelay":"00000000"}

  render_controller_io_stop_timeout_timer();
  if (is_amount_cmd_wrong(input))
    END_HANDLE_WITH_ERROR();

  uint16_t frames_amount = STR_TO_NUM(input->data + 17);
  if (frames_amount > MAX_FRAMES_AMOUNT)
    END_HANDLE_WITH_ERROR();

  uint32_t delay = convert_str_to_delay(input->data + 41);
  if (delay > 86400000U)
    END_HANDLE_WITH_ERROR();
  render_controller_set_delay(delay);
  
  frame_buffer_reset();
  
  remaining_frames = frames_amount;
  frame_buffer_set_frames_amount(frames_amount);

  handler_queue_add(handle_seq_data);

  hc06_read((uint8_t*)input->data, displays_conf_get_pixels_num() * 3 + 12);
  render_controller_io_start_timeout_timer();

  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));
}

static void set_handlers(handler self, handler_input *const input)
{
  handler_queue_clear();
  remaining_frames = 0;

  handler_queue_add(handle_frames_amount);

  hc06_read((uint8_t*)input->data, 51);
  render_controller_io_start_timeout_timer();

  hc06_write((uint8_t*)OK_STRING, strlen(OK_STRING));
  DEBUG_OUTPUT("seq mode handler set", strlen("seq mode handler set"));
}

static void destroy(handler self)
{
  free(self);
}

// Static variables ----------------------------------------------------------

static handler_interface_struct interface =
{
  set_handlers,
  destroy
};

// Implementations -----------------------------------------------------------

handler seq_mode_handler_create()
{
  handler handler = malloc(sizeof(handler_struct));
  handler->name = "SEQ";
  handler->vtable = &interface;

  return handler;
}