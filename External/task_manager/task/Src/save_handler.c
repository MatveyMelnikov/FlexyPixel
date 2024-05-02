#include "save_handler.h"
// #include "hc06_driver.h"
// #include "render_controller.h"
// #include "render_controller_io.h"
// #include "render_controller_defs.h"
// #include "handler_queue.h"
// #include "led_panels_driver.h"
// #include "list_of_changes.h"
// #include "displays_conf.h"
#include "debug_output.h"
#include "frame_buffer.h"
#include "list_of_changes.h"
// #include <string.h>
// #include <stdlib.h>
// #include <stdint.h>

// Defines -------------------------------------------------------------------

// #define END_HANDLE_WITH_ERROR() \
//   { \
//     hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING)); \
//     return; \
//   }

// Static variables ----------------------------------------------------------

// enum {
//   FRAME_FIELD_OFFSET = 2U,
//   FRAMES_AMOUNT_FIELD_OFFSET = 2U,
//   FRAMES_AMOUNT_OFFSET = 17U,
//   DELAY_OFFSET = 41U,
//   DATA_OFFSET = 10U,
//   START_CMD_LENGTH = 51U,
// };

// static int16_t remaining_frames = 0;

// Static functions ----------------------------------------------------------

// static uint32_t convert_str_to_delay(const uint8_t *const str)
// {
//   uint32_t result = 0;
//   uint32_t delimeter = 10000000;
//   for (uint8_t i = 0; i < 8; i++)
//   {
//     result += (uint32_t)CHAR_TO_NUM(str + i) * delimeter;
//     delimeter /= 10;
//   }

//   return result;
// }

// static bool is_seq_cmd_wrong(handler_input *const input)
// {
//   return !CHECK_STR(
//     input->data + FRAME_FIELD_OFFSET, "frame", strlen("frame")
//   );
// }

// static bool is_amount_cmd_wrong(handler_input *const input)
// {
//   bool is_field_wrong = !CHECK_STR(
//     input->data + FRAMES_AMOUNT_FIELD_OFFSET,
//     "framesAmount",
//     strlen("framesAmount")
//   );
//   return (displays_conf_is_empty() || is_field_wrong);
// }

// static bool are_frames_left()
// {
//   return --remaining_frames > 0;
// }

// Blocking task
static void handle_save(handler_input *const input)
{
  // Symbols: 204 (for 8x8 panels)
  // {"frame":"999000777...987"}

  //render_controller_io_stop_timeout_timer();

  // if (!handler_queue_get_hold_flag()) // First call
  // {
  //   if (is_seq_cmd_wrong(input))
  //     END_HANDLE_WITH_ERROR();

  //   frame_buffer_set(input->data + DATA_OFFSET);
  //   frame_buffer_save();
  //   handler_queue_set_hold(true);
  //   return;
  // }

  // if (frame_buffer_is_busy()) // Page is not saved
  //   return;
  // if (frame_buffer_save()) // Not all data saved
  //   return;

  // handler_queue_set_hold(false);

  // if (are_frames_left())
  // {
  //   handler_queue_skip_remove();
  //   hc06_read((uint8_t*)input->data, displays_conf_get_pixels_num() * 3 + 12);
  //   render_controller_io_start_timeout_timer();
  //   hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));
  //   return;
  // }

  // list_of_changes_clear();
  // frame_buffer_lock(false);
  // handler_queue_clear();
  // hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));



  // frame_buffer_status status;

  // frame_buffer_prepare_save();
  // do {
  //   status = frame_buffer_internal_load();
  //   if (status)
  //     goto error;
  //   frame_buffer_apply_changes();

  //   do {
  //     status = frame_buffer_save();
  //   } while (status == FRAME_BUFFER_IN_PROGRESS);

  //   if (status)
  //     goto error;
  // } while (!frame_buffer_is_all_data_saved());

  return;

error:
  DEBUG_OUTPUT("diconnect save error", strlen("diconnect save error"));
}

// static void handle_frames_amount(handler_input *const input)
// {
//   // Symbols: 51
//   // {"framesAmount":"030","interframeDelay":"00000000"}

//   // render_controller_io_stop_timeout_timer();
//   // if (is_amount_cmd_wrong(input))
//   //   END_HANDLE_WITH_ERROR();

//   // uint16_t frames_amount = STR_TO_NUM(input->data + FRAMES_AMOUNT_OFFSET);
//   // if (frames_amount > MAX_FRAMES_AMOUNT)
//   //   END_HANDLE_WITH_ERROR();

//   // uint32_t delay = convert_str_to_delay(input->data + DELAY_OFFSET);
  
//   // frame_buffer_reset();
  
//   // frame_buffer_lock(true);
//   // frame_buffer_set_render_delay(delay);

//   // remaining_frames = frames_amount;
//   // frame_buffer_set_frames_amount(frames_amount);

//   // handler_queue_add(handle_seq_data);

//   // hc06_read((uint8_t*)input->data, displays_conf_get_pixels_num() * 3 + 12);
//   // render_controller_io_start_timeout_timer();

//   // hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));
// }

static void set_handlers(handler self, handler_input *const input)
{
  handler_queue_clear();
  // remaining_frames = 0;

  // handler_queue_add(handle_frames_amount);

  // hc06_read((uint8_t*)input->data, START_CMD_LENGTH);
  // render_controller_io_start_timeout_timer();

  // hc06_write((uint8_t*)OK_STRING, strlen(OK_STRING));
  // DEBUG_OUTPUT("seq mode handler set", strlen("seq mode handler set"));

  handle_save(input);
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

handler save_handler_create()
{
  handler handler = malloc(sizeof(handler_struct));
  handler->name = "SAVE";
  handler->vtable = &interface;

  return handler;
}