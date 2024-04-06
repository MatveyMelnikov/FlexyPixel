#include "seq_mode_handler.h"
#include "hc06_driver.h"
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

// Defines -------------------------------------------------------------------

#define END_HANDLE_WITH_ERROR() \
  { \
    hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING)); \
    return; \
  }

// Static variables ----------------------------------------------------------

static int16_t remaining_frames = 0;
static uint8_t current_display = 0;

// Static functions ----------------------------------------------------------

static bool is_seq_cmd_wrong(handler_input *const input)
{
  return !CHECK_STR(input->data + 2, "frame", 5);
}

static bool is_amount_cmd_wrong(handler_input *const input)
{
  bool is_field_wrong = !CHECK_STR(input->data + 2, "framesAmount", 12);
  return (displays_conf_is_empty() || is_field_wrong);
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

    frame_buffer_set(input->data + 10);
    frame_buffer_save();
    handler_queue_set_hold(true);
    return;
  }

  if (!frame_buffer_is_saved()) // Page is not saved
    return;
  if (frame_buffer_save()) // Not all data saved
    return;

  handler_queue_set_hold(false);
  //hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  if (remaining_frames == 2)
    __asm("nop");

  if (--remaining_frames > 0)
  {
    //handler_queue_add(handle_seq_data);
    handler_queue_skip_remove();
    hc06_read((uint8_t*)input->data, displays_conf_get_pixels_num() * 3 + 12);
    render_controller_io_start_timeout_timer();
    hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));
    return;
  }

  handler_queue_clear();
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));
}

static void handle_frames_amount(handler_input *const input)
{
  // Symbols: 23
  // {"framesAmount":"030"}

  render_controller_io_stop_timeout_timer();
  if (is_amount_cmd_wrong(input))
    END_HANDLE_WITH_ERROR();

  uint8_t frames_amount = STR_TO_NUM(input->data + 17);

  if (frames_amount > MAX_FRAMES_AMOUNT)
    END_HANDLE_WITH_ERROR();
  
  remaining_frames = frames_amount;
  frame_buffer_set_frames_num(frames_amount);

  // if (!displays_conf_is_panel_configured(current_display))
  //   END_HANDLE_WITH_ERROR();

  handler_queue_add(handle_seq_data);

  //uint16_t bytes_to_read = displays_conf_get()[current_display] * 3 + 12;
  hc06_read((uint8_t*)input->data, displays_conf_get_pixels_num() * 3 + 12);
  render_controller_io_start_timeout_timer();

  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));
}

static void set_handlers(handler self, handler_input *const input)
{
  handler_queue_clear();
  remaining_frames = 0;
  //current_display = 0;

  handler_queue_add(handle_frames_amount);

  hc06_read((uint8_t*)input->data, 22);
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