#include "render_controller.h"
#include "render_controller_defs.h"
#include "mode_handler.h"
#include "hc06_driver.h"
#include "led_panels_driver.h"
#include "handler_queue.h"
#include <memory.h>

// Defines -------------------------------------------------------------------

#define IS_COMMAND(buf, cmd) \
  memcmp(buf + 9, cmd, 4 * sizeof(char)) == 0

// Static variables ----------------------------------------------------------

static mode_handler current_handlers[MODES_NUM] = { NULL };
static uint8_t io_buffer[INPUT_BUFFER_SIZE];
static uint16_t display_configuration[9];
// Заменить на модуль типа handler_queue
//void (*handle_function)(uint8_t *const, led_panels_buffer *const) = NULL;


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
  // 66 symbols
  // {"sizes": ["256","256","256","256","256","256","256","256","256"]}
  UNUSED(buffer);

  for (uint16_t i = 0; i < 9; i++)
  {
    display_configuration[i] = string_to_num(
      (char *)data + 12 + (6 * i)
    );
  }

  //handle_function = NULL;
  handler_queue_clear();
}

static void set_configuration(void)
{
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  handler_queue_add(receive_configuration);
}

static void receive_command(void)
{
  // {"type":"conf"} ("mode"/"data") - 15 symbols
  if (!hc06_is_data_received())
    return;

  if (IS_COMMAND(io_buffer, "conf"))
    set_configuration();

  hc06_read(io_buffer, 66);
}

// Implemantations -----------------------------------------------------------

void render_controller_create(
  mode_handler *const handlers,
  uint8_t handlers_num
)
{
  memcpy(current_handlers, handlers, handlers_num * sizeof(mode_handler));
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