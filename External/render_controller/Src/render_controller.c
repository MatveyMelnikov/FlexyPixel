#include "render_controller.h"
#include "render_controller_defs.h"
#include "render_controller_io.h"
#include "mode_handler.h"
#include "hc06_driver.h"
#include "led_panels_driver.h"
#include "handler_queue.h"
#include "handler_input.h"
#include <memory.h>
#include <stddef.h>

// Static variables ----------------------------------------------------------

static mode_handler modes[MODES_NUM] = { NULL }; // modes num + handlers_num?
static uint8_t io_buffer[INPUT_BUFFER_SIZE];
static led_panels_size display_configuration[DISPLAY_NUM]; // to led_panels_sizes
static led_panels_buffer *front_buffer = NULL;
static led_panels_buffer *back_buffer = NULL;
static handler_input handler_args = { 0 };
static mode_handler current_mode = NULL;

// Static functions ----------------------------------------------------------

static bool is_configuration_empty()
{
  return display_configuration[0] == 0;
}

static void receive_configuration(handler_input *const input)
{
  // 73 symbols
  // {"configuration":["256","256","256","256","256","256","256","256","256"]}

  render_controller_io_stop_timeout_timer();
  if (is_first_field_not_suitable(io_buffer, "configuration"))
    return;

  bool is_ok = true;
  led_panels_size new_configuration[DISPLAY_NUM] = { 0 };
  uint8_t current_displays_num = 0;
  for (; current_displays_num < DISPLAY_NUM; current_displays_num++)
  {
    uint16_t display_size = STR_TO_NUM(
      (char *)input->data + CONFIGURATION_OFFSET + (6 * current_displays_num)
    );
    if (display_size == 0)
      break;
    if (
      display_size != LED_PANELS_SIZE_64 && 
      display_size != LED_PANELS_SIZE_256
    )
    {
      is_ok = false;
      break;
    }

    new_configuration[current_displays_num] = display_size;
  }

  if (is_ok)
  {
    memcpy(
      display_configuration, new_configuration, sizeof(uint16_t) * DISPLAY_NUM
    );
    led_panels_destroy(front_buffer);
    led_panels_destroy(back_buffer);
    
    front_buffer = led_panels_create(
      current_displays_num,
      display_configuration
    );
    back_buffer = led_panels_create(
      current_displays_num,
      display_configuration
    );
  }
  send_status(is_ok);
}

static void receive_mode(handler_input *const input)
{
  // 14 symbols
  // {"mode":"SEQ"} (PIX)

  render_controller_io_stop_timeout_timer();
  if (is_first_field_not_suitable(io_buffer, "mode"))
    return;

  bool is_ok = false;
  for (uint8_t i = 0; i < MODES_NUM; i++)
  {
    if (CHECK_STR(input->data + MODE_OFFSET, modes[i]->mode_name, MODE_LEN))
    {
      current_mode = modes[i];
      is_ok = true;
      break;
    }
  }

  send_status(is_ok);
}

static void set_configuration_handlers(
  void (*handle_function)(handler_input *const),
  uint16_t number_of_chars_read
)
{
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  handler_queue_add(handle_function);
  hc06_read(io_buffer, number_of_chars_read);
  render_controller_io_start_timeout_timer();
}

static void set_data_handlers(void)
{
  if (current_mode == NULL || is_configuration_empty())
  {
    hc06_write((uint8_t *)UNCONFIGURED_STRING, strlen(UNCONFIGURED_STRING));
    hc06_read(io_buffer, CMD_LEN);
    return;
  }
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  mode_handler_set_handlers(current_mode, &handler_args);
}

static void receive_command(void)
{
  // {"type":"conf"} ("mode"/"data") - 15 symbols

  if (!CHECK_STR(io_buffer + FIRST_FIELD_OFFSET, "type", strlen("type")))
    goto error;

  if (CHECK_STR(io_buffer + CMD_TYPE_OFFSET, "conf", strlen("conf")))
  {
    set_configuration_handlers(receive_configuration, 73);
    return;
  }
  if (CHECK_STR(io_buffer + CMD_TYPE_OFFSET, "mode", strlen("mode")))
  {
    set_configuration_handlers(receive_mode, 14);
    return;
  }
  if (CHECK_STR(io_buffer + CMD_TYPE_OFFSET, "data", strlen("data")))
  {
    set_data_handlers();
    return;
  }
  
error:
  hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
  hc06_read(io_buffer, CMD_LEN);
}

// Implemantations -----------------------------------------------------------

void render_controller_create(
  mode_handler *const handlers,
  uint8_t handlers_num
)
{
  hc06_set_baudrate(HC06_115200);
  memcpy(modes, handlers, handlers_num * sizeof(mode_handler));

  handler_args.buffer = &front_buffer,
  handler_args.configurations = display_configuration,
  handler_args.data = io_buffer;

  hc06_read(io_buffer, CMD_LEN);
  render_controller_io_create(front_buffer);
}

void render_controller_destroy(void)
{
  handler_args.buffer = NULL,
  handler_args.configurations = NULL,
  handler_args.data = NULL;

  memset(display_configuration, 0, sizeof(uint16_t) * DISPLAY_NUM);

  led_panels_destroy(front_buffer);
  front_buffer = NULL;
  led_panels_destroy(back_buffer);
  back_buffer = NULL;

  current_mode = NULL;

  render_controller_io_destroy();
}

bool render_controller_process()
{
  if (hc06_is_data_received())
  {
    if (!handler_queue_is_empty()) {
      handler_queue_run(&handler_args);
      if (handler_queue_is_empty())
        hc06_read(io_buffer, CMD_LEN);
    } else {
      receive_command();
    }
  }

  if (render_controller_io_is_timeout())
  {
    render_controller_io_stop_timeout_timer();
    hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
    handler_queue_clear();
  }

  // render

  return true;
}
