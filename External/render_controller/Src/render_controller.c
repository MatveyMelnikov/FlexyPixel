#include "render_controller.h"
#include "render_controller_defs.h"
#include "render_controller_io.h"
#include "displays_conf.h"
#include "task_manager.h"
#include "task.h"
#include "hc06_driver.h"
#include "led_panels_driver.h"
#include "task_input.h"
#include "list_of_changes.h"
#include "frame_buffer.h"
#include "debug_output.h"
#include <memory.h>
#include <stddef.h>

// Static variables ----------------------------------------------------------

static uint8_t io_buffer[INPUT_BUFFER_SIZE];
static led_panels_buffer *front_buffer = NULL;
static led_panels_buffer *back_buffer = NULL;
static task_input handler_args = { 0 };
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
  
  DEBUG_OUTPUT("configuration set", strlen("configuration set"));
  front_buffer = led_panels_create(
    displays_conf_get_displays_num(),
    displays_conf_get()
  );
  back_buffer = led_panels_create(
    displays_conf_get_displays_num(),
    displays_conf_get()
  );

  displays_conf_reset_update_flag();
}

static void render()
{
  if (front_buffer->is_locking || frame_buffer_is_locked())
    return;

  // swap buffer
  led_panels_buffer *tmp = front_buffer;
  front_buffer = back_buffer;
  back_buffer = tmp;

  led_panels_send(front_buffer);

  led_panels_copy_data(back_buffer, front_buffer);
  
  frame_buffer_load(back_buffer);
  list_of_changes_apply_changes(back_buffer);
}

static bool is_disconnect_caught()
{
  if (!CHECK_STR(io_buffer, "+DISC:SUCC", strlen("+DISC:SUCC")))
    return false;

  return task_manager_set("SAVE");
}

static void receive_command(void)
{
  // {"type":"CONF"} ("MODE"/"DATA") - 15 symbols

  if (is_disconnect_caught())
    return;

  if (!CHECK_STR(io_buffer + FIRST_FIELD_OFFSET, "type", strlen("type")))
  {
    hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
    goto end;
  }

  if (task_manager_set((char*)(io_buffer + CMD_TYPE_OFFSET)))
    return;

  handler_args.is_input_changed = false;

end:
  hc06_read(io_buffer, CMD_LEN);
}

static void process_input()
{
  if (!hc06_is_data_received())
    return;
  DEBUG_OUTPUT((char*)io_buffer, 70);

  handler_args.is_input_changed = true;
  if (task_manager_is_empty()) {
    receive_command();
    return;
  }
}

static void process_tasks()
{
  if (task_manager_is_empty())
    return;

  task_manager_run(&handler_args);
  handler_args.is_input_changed = false;

  if (task_manager_is_empty())
    hc06_read(io_buffer, CMD_LEN);
}

static bool is_time_to_render()
{
  if (list_of_changes_is_updated())
    return true;

  return (
    (render_controller_io_get_ticks() - captured_ticks) >=
      frame_buffer_get_render_delay()
  );
}

// Implemantations -----------------------------------------------------------

void render_controller_create(
  task *const tasks_list,
  uint8_t tasks_amount
)
{
  hc06_set_baudrate(HC06_115200);

  task_manager_add_tasks_list(tasks_list, tasks_amount);

  handler_args.data = io_buffer;
  handler_args.is_input_changed = false;

  hc06_read(io_buffer, CMD_LEN);
  render_controller_io_create(&front_buffer);

  frame_buffer_load_conf();
  set_configuration();
  list_of_changes_load();

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

  task_manager_reset();
  render_controller_io_destroy();
}

bool render_controller_process()
{
  process_input();
  process_tasks();

  // if (render_controller_io_is_timeout())
  // {
  //   render_controller_io_stop_timeout_timer();
  //   hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
  //   handler_queue_clear();
  // }

  set_configuration();

  // render
  if (front_buffer == NULL)
    return false;

  if (!is_time_to_render())
    return true;

  captured_ticks = render_controller_io_get_ticks();
  render();

  return true;
}
