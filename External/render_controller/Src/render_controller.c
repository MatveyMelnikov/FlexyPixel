#include "render_controller.h"
#include "render_controller_defs.h"
#include "mode_handler.h"
#include "hc06_driver.h"
#include "led_panels_driver.h"
#include "handler_queue.h"
#include "handler_input.h"
#include <memory.h>

// Defines -------------------------------------------------------------------

#define CHECK_STR(str_a, str_b, size) \
  memcmp((str_a), (str_b), (size) * sizeof(char)) == 0

// Static variables ----------------------------------------------------------

static mode_handler modes[MODES_NUM] = { NULL }; // modes num + handlers_num?
static uint8_t io_buffer[INPUT_BUFFER_SIZE];
static uint16_t display_configuration[9];
static led_panels_buffer front_buffer;
static led_panels_buffer end_buffer;
static handler_input handler_args = (handler_input) {
  .buffer = &front_buffer,
  .configurations = display_configuration,
  .data = io_buffer
};
mode_handler current_mode = NULL;

// Static functions ----------------------------------------------------------

// static uint16_t string_to_num(char *str)
// {
//   return ((uint16_t)(*str) - '0') * 100 + 
//     ((uint16_t)(*(str + 1)) - '0') * 10 +
//     ((uint16_t)(*(str + 2)) - '0');
// }

static bool is_configuration_empty()
{
  return display_configuration[0] != 0;
}

static void receive_configuration(handler_input *const input)
{
  // 73 symbols
  // {"configuration":["256","256","256","256","256","256","256","256","256"]}

  bool is_ok = true;
  for (uint16_t i = 0; i < 9; i++)
  {
    uint16_t display_size = STR_TO_NUM(
      (char *)input->data + 19 + (6 * i)
    );
    if (display_size != 64 && display_size != 256)
    {
      is_ok = false;
      break;
    }

    display_configuration[i] = display_size;
  }

  //handler_queue_clear();
  if (is_ok)
    hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));
  else
    hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
}

static void receive_mode(handler_input *const input)
{
  // 14 symbols
  // {"mode":"SEQ"} (PIX)

  bool is_ok = false;
  for (uint8_t i = 0; i < MODES_NUM; i++)
  {
    if (CHECK_STR(input->data + 9, modes[i]->mode_name, 3))
    {
      current_mode = modes[i];
      is_ok = true;
    }
  }

  //handler_queue_clear();
  if (is_ok)
    hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));
  else
    hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
}

static void set_configuration(void)
{
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  handler_queue_add(receive_configuration);
  hc06_read(io_buffer, 73);
}

static void set_mode(void)
{
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  handler_queue_add(receive_mode);
  hc06_read(io_buffer, 14);
}

static void set_data_handlers(void)
{
  if (current_mode == NULL || is_configuration_empty())
  {
    hc06_write((uint8_t *)UNCONFIGURED_STRING, strlen(UNCONFIGURED_STRING));
    return;
  }
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  mode_handler_set_handlers(current_mode, &handler_args);
}

static void receive_command(void)
{
  // {"type":"conf"} ("mode"/"data") - 15 symbols
  // if (!hc06_is_data_received())
  //   return;

  if (CHECK_STR(io_buffer + 9, "conf", 4))
  {
    set_configuration();
    return;
  }
  if (CHECK_STR(io_buffer + 9, "mode", 4))
  {
    set_mode();
    return;
  }
  if (CHECK_STR(io_buffer + 9, "data", 4))
  {
    set_data_handlers();
    return;
  }
  
  hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
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
    if (!handler_queue_is_empty())
      handler_queue_run(&handler_args);
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