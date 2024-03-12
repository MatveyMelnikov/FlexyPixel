#include "render_controller.h"
#include "render_controller_defs.h"
#include "mode_handler.h"
#include "hc06_driver.h"
#include "led_panels_driver.h"
#include "handler_queue.h"
#include <memory.h>

// Defines -------------------------------------------------------------------

#define CHECK_STR(buf, cmd) \
  memcmp(buf + 9, cmd, 4 * sizeof(char)) == 0

// Static variables ----------------------------------------------------------

static mode_handler modes[MODES_NUM] = { NULL }; // modes num + handlers_num?
static uint8_t io_buffer[INPUT_BUFFER_SIZE];
static uint16_t display_configuration[9];
mode_handler *current_mode = NULL;

// Static functions ----------------------------------------------------------

static uint16_t string_to_num(char *str)
{
  return ((uint16_t)(*str) - '0') * 100 + 
    ((uint16_t)(*(str + 1)) - '0') * 10 +
    ((uint16_t)(*(str + 2)) - '0');
}

static void receive_configuration(
  uint8_t *const data,
  led_panels_buffer *const buffer
)
{
  // 65 symbols
  // {"sizes":["256","256","256","256","256","256","256","256","256"]}
  UNUSED(buffer);

  for (uint16_t i = 0; i < 9; i++)
  {
    display_configuration[i] = string_to_num(
      (char *)data + 11 + (6 * i)
    );
  }

  //handle_function = NULL;
  handler_queue_clear();
}

static void receive_mode(
  uint8_t *const data,
  led_panels_buffer *const buffer
)
{
  // 14 symbols
  // {"mode":"IMG"} (GIF / RTP)

  UNUSED(buffer);

  // if (CHECK_STR(buffer + 9, "IMG"))
  //   current_mode = 

  for (uint8_t i = 0; i < MODES_NUM; i++)
  {
    if (CHECK_STR(buffer + 9, modes[i]->mode_name))
      current_mode = modes[i];
  }
}

static void set_configuration(void)
{
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  handler_queue_add(receive_configuration);
  hc06_read(io_buffer, 65);
}

static void set_mode(void)
{
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  handler_queue_add(receive_mode);
  hc06_read(io_buffer, 65);
}

static void set_data_handlers(void)
{
  if (current_mode == NULL)
  {
    hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
    return;
  }
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  mode_handler_set_handlers(current_mode, io_buffer);
}

static void receive_command(void)
{
  // {"type":"conf"} ("mode"/"data") - 15 symbols
  if (!hc06_is_data_received())
    return;

  if (CHECK_STR(io_buffer, "conf"))
    set_configuration();
  if (CHECK_STR(io_buffer, "mode"))
    set_mode();
  if (CHECK_STR(io_buffer, "data"))
    set_data_handlers();  

  hc06_read(io_buffer, 15);
}

// Implemantations -----------------------------------------------------------

void render_controller_create(
  mode_handler *const handlers,
  uint8_t handlers_num
)
{
  memcpy(modes, handlers, handlers_num * sizeof(mode_handler));
  hc06_read(io_buffer, 15);
}

HAL_StatusTypeDef render_controller_process()
{
  if (hc06_is_data_received())
  {
    // if (handle_function != NULL)
    //   handle_function(io_buffer, NULL);
    // if (handle_function != NULL)
    //   receive_command();
    if (!handler_queue_is_empty())
      handler_queue_run(io_buffer, NULL);
    if (handler_queue_is_empty())
      receive_command();
  }

  // render

  return HAL_OK;
}

void render_controller_send_frame_complete()
{
  //led_panels_send_complete(buffer);
}

void render_controller_receive_complete()
{
  hc06_receive_complete();
}