#include "render_controller.h"
#include "render_controller_defs.h"
#include "render_controller_io.h"
#include "displays_conf.h"
#include "handler_list.h"
#include "handler.h"
#include "hc06_driver.h"
#include "led_panels_driver.h"
#include "handler_queue.h"
#include "handler_input.h"
#include "list_of_changes.h"
#include <memory.h>
#include <stddef.h>

// Static variables ----------------------------------------------------------

//static handler modes[MODES_NUM] = { NULL };
static uint8_t io_buffer[INPUT_BUFFER_SIZE];
static bool pixels_have_changed = false;
static led_panels_buffer *front_buffer = NULL;
static led_panels_buffer *back_buffer = NULL;
static handler_input handler_args = { 0 };
static uint32_t captured_ticks = 0U;

// Static functions ----------------------------------------------------------

static void set_configuration()
{
  if (!displays_conf_is_updated())
    return;
  if (front_buffer != NULL && front_buffer->is_locking)
    return;

  led_panels_destroy(front_buffer);
  led_panels_destroy(back_buffer);
  
  front_buffer = led_panels_create(
    displays_conf_get_displays_num(),
    displays_conf_get()
  );
  back_buffer = led_panels_create(
    displays_conf_get_displays_num(),
    displays_conf_get()
  );

  list_of_changes_destroy();
  list_of_changes_create(displays_conf_get_pixels_num());

  displays_conf_reset_update_flag();
}

static void fill_back_buffer()
{
  pixel_change change;
  while (list_of_changes_get(&change))
  {
    (void)led_panels_set_pixel(
      back_buffer,
      change.panel_position,
      change.x,
      change.y,
      change.color
    );

    pixels_have_changed = true;
  }

  list_of_changes_clear();
}

static void render()
{
  if (front_buffer->is_locking || !pixels_have_changed)
    return;

  // swap buffer
  led_panels_buffer *tmp = front_buffer;
  front_buffer = back_buffer;
  back_buffer = tmp;

  led_panels_send(front_buffer);

  led_panels_copy_pwm_data(back_buffer, front_buffer);
  
  pixels_have_changed = false;
}

static void receive_command(void)
{
  // {"type":"CONF"} ("MODE"/"DATA") - 15 symbols

  if (!CHECK_STR(io_buffer + FIRST_FIELD_OFFSET, "type", strlen("type")))
    goto error;
  
  const char *handler_name = NULL;
  for (uint8_t i = 0; i < HANDLERS_NUM; i++)
  {
    handler_name = handler_list_get_name(i);
    if (handler_name == NULL)
      break;
    
    if (CHECK_STR(
      io_buffer + CMD_TYPE_OFFSET, handler_name, strlen(handler_name)
    ))
    {
      handler_list_set(i, &handler_args);
      return;
    }
  }
  
error:
  hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
  hc06_read(io_buffer, CMD_LEN);
}

// Implemantations -----------------------------------------------------------

void render_controller_create(
  handler *const handlers,
  uint8_t handlers_num
)
{
  hc06_set_baudrate(HC06_115200);

  handler_list_add(handlers, handlers_num);

  handler_args.data = io_buffer;

  hc06_read(io_buffer, CMD_LEN);
  render_controller_io_create(&front_buffer);

  captured_ticks = render_controller_io_get_ticks();
}

void render_controller_destroy(void)
{
  handler_args.data = NULL;

  displays_conf_clear();

  led_panels_destroy(front_buffer);
  front_buffer = NULL;
  led_panels_destroy(back_buffer);
  back_buffer = NULL;

  handler_list_destroy();
  render_controller_io_destroy();
}

bool render_controller_process()
{
  // input
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

  set_configuration();

  // render
  if (front_buffer == NULL)
    return false;
  fill_back_buffer();

  if ((render_controller_io_get_ticks() - captured_ticks) < RENDER_DELAY)
    return true;

  captured_ticks = render_controller_io_get_ticks();
  render();

  return true;
}
