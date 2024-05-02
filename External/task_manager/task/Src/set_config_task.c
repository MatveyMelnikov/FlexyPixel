#include "set_config_task.h"
#include "render_controller_io.h"
#include "render_controller_defs.h"
#include "displays_conf.h"
#include "debug_output.h"
#include "list_of_changes.h"
#include <stdlib.h>

// Defines -------------------------------------------------------------------

enum
{
  CONF_OFFSET = 18U,
  CONF_CMD_LENGTH = 47U,
};

// Static variables ----------------------------------------------------------

static task_interface_struct interface;

// Static functions ----------------------------------------------------------

static task_output handle_configuration(task_input *const input)
{
  // 47 symbols
  // {"configuration":"256064000000000000000000000"}

  if (is_first_field_not_suitable(input->data, "configuration"))
  {
    return (task_output) {
      .status = EXECUTION_ERROR,
      .response = RESPONSE_ERROR,
      .request_size = 0U,
      .start_next_task_immediately = false
    };
  }

  bool success = true;
  led_panels_size new_configuration[CONFIGURATION_SIZE] = { 0 };
  uint8_t displays_num = 0;
  for (; displays_num < CONFIGURATION_SIZE; displays_num++)
  {
    uint16_t display_size = STR_TO_NUM(
      (char *)input->data + CONF_OFFSET + (3 * displays_num)
    );
    if (display_size == 0)
      break;
    if (
      display_size != LED_PANELS_SIZE_64 && 
      display_size != LED_PANELS_SIZE_256
    )
    {
      success = false;
      break;
    }

    new_configuration[displays_num] = display_size;
    list_of_changes_clear();
  }

  if (success)
    displays_conf_update(new_configuration, displays_num);

  return (task_output) {
    .status = success ? EXECUTION_COMPLETED : EXECUTION_ERROR,
    .response = success ? RESPONSE_OK : RESPONSE_ERROR,
    .request_size = 0U,
    .start_next_task_immediately = false
  };
}

static task_output set_handler(task_input *const input)
{
  return (task_output) {
    .status = EXECUTION_COMPLETED,
    .response = RESPONSE_OK,
    .request_size = CONF_CMD_LENGTH,
    .start_next_task_immediately = false
  };
}

static void destroy(task self)
{
  task_destroy(self);
  free(self);
}

// Implementations -----------------------------------------------------------

task set_config_task_create()
{
  task set_conf_task = malloc(sizeof(task_struct));
  set_conf_task->name = "CONF";
  set_conf_task->vtable = &interface;
  set_conf_task->ttl = 0;

  interface.destroy = destroy;
  task_create_subtasks_list(set_conf_task, 2);
  interface.subtasks[0] = set_handler;
  interface.subtasks[1] = handle_configuration;

  return set_conf_task;
}
