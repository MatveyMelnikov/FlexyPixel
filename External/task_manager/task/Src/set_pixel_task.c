#include "set_pixel_task.h"
#include "task_input.h"
#include "hc06_driver.h"
#include "render_controller_io.h"
#include "render_controller_defs.h"
#include "led_panels_driver.h"
#include "list_of_changes.h"
#include "displays_conf.h"
#include "debug_output.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// Defines -------------------------------------------------------------------

enum {
  RED_OFFSET = 33U,
  GREEN_OFFSET = 34U,
  BLUE_OFFSET = 35U,
  PANEL_POS_OFFSET = 17U,
  PIXEL_POS_OFFSET = 55,
  PANEL_POS_FIELD_OFFSET = 2U,
  PIXEL_POS_FIELD_OFFSET = 39U,
  PIXEL_CMD_LENGTH = 60U,
};

#define END_HANDLE_WITH_ERROR() \
  { \
    return (task_output) { \
      .status = EXECUTION_ERROR, \
      .response = RESPONSE_ERROR, \
      .request_size = 0U, \
      .start_next_task_immediately = false \
    }; \
  }

// Static variables ----------------------------------------------------------

static task_interface_struct interface;

// Static functions ----------------------------------------------------------

static bool is_transmit_wrong(task_input *const input)
{
  bool is_first_field = !CHECK_STR(
    input->data + PANEL_POS_FIELD_OFFSET,
    "panelPosition",
    strlen("panelPosition")
  );
  bool is_last_field = !CHECK_STR(
    input->data + PIXEL_POS_FIELD_OFFSET,
    "pixelPosition",
    strlen("pixelPosition")
  );
  return (displays_conf_is_empty() || is_first_field || is_last_field);
}

static task_output handle_pixel_data(task_input *const input)
{
  // Symbols: 60
  // {"panelPosition":9,"pixelColor":"967","pixelPosition":"000"}

  if (is_transmit_wrong(input))
    END_HANDLE_WITH_ERROR();

  uint8_t panel_position = CHAR_TO_NUM(input->data + PANEL_POS_OFFSET);
  led_panels_color color = (led_panels_color) {
    .red = CHAR_TO_NUM(input->data + RED_OFFSET),
    .green = CHAR_TO_NUM(input->data + GREEN_OFFSET),
    .blue = CHAR_TO_NUM(input->data + BLUE_OFFSET)
  };
  uint8_t pixel_position = STR_TO_NUM(input->data + PIXEL_POS_OFFSET);

  if (!displays_conf_is_panel_configured(panel_position))
    END_HANDLE_WITH_ERROR();

  bool success = list_of_changes_add(
    panel_position,
    pixel_position,
    color
  );
  
  return (task_output) {
    .status = success ? EXECUTION_COMPLETED : EXECUTION_ERROR,
    .response = success ? RESPONSE_OK : RESPONSE_ERROR,
    .request_size = 0U,
    .start_next_task_immediately = false
  };
}

static task_output set_handlers(task_input *const input)
{
  return (task_output) {
    .status = EXECUTION_COMPLETED,
    .response = RESPONSE_OK,
    .request_size = PIXEL_CMD_LENGTH,
    .start_next_task_immediately = false
  };
}

static void destroy(task self)
{
  task_destroy(self);
  free(self);
}

// Implementations -----------------------------------------------------------

task set_pixel_task_create()
{
  task set_pixel_task = malloc(sizeof(task_struct));
  set_pixel_task->name = "PIX";
  set_pixel_task->vtable = &interface;
  set_pixel_task->ttl = 0;

  interface.destroy = destroy;
  task_create_subtasks_list(set_pixel_task, 2);
  interface.subtasks[0] = set_handlers;
  interface.subtasks[1] = handle_pixel_data;

  return set_pixel_task;
}