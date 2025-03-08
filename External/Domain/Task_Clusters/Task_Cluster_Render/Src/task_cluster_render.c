#include "task.h"
#include "task_cluster_render.h"
#include "render_controller.h"
#include "displays_config_repo.h"
#include <stdlib.h>
#include <string.h>

// Defines -------------------------------------------------------------------

// Static variables ----------------------------------------------------------

static task_interface_struct interface_init;
static task_interface_struct interface_render;
static uint32_t current_tick = 0U;

// Static functions ----------------------------------------------------------

static task_output task_init(task_arg *const argument);
static task_output task_render(task_arg *const argument);
static void task_render_destroy(task_ptr self);

// Implementations -----------------------------------------------------------

task_cluster task_cluster_render_create()
{
  interface_init = (task_interface_struct) {
    .run = task_init,
    .destroy = task_render_destroy
  };
  interface_render = (task_interface_struct) {
    .run = task_render,
    .destroy = task_render_destroy
  };

  static task cluster_tasks[] = {
    (task) {
      .ttl = 0U,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_init
    },
    (task) {
      .ttl = 0U,
      .start_tick = 0U,
      .is_infinite = true,
      .vtable = &interface_render
    }
  };

  return (task_cluster) {
    .name = TASK_CLUSTER_RENDER_NAME,
    .tasks = cluster_tasks,
    .tasks_amount = (sizeof(cluster_tasks) / sizeof(task))
  };
}

static task_output task_init(task_arg *const argument)
{
  render_controller_create();
  current_tick = argument->current_tick;

  return TASK_OUTPUT_COMPLETED;
}

static task_output task_render(task_arg *const argument)
{
  const displays_config_repo_external_data *config =
    displays_config_repo_get();

  if ((argument->current_tick - current_tick) < config->render_delay)
    return TASK_OUTPUT_IN_PROGRESS;
  
  (void)render_controller_render();
  current_tick = argument->current_tick;

  return TASK_OUTPUT_IN_PROGRESS;
}

static void task_render_destroy(task_ptr self)
{
  current_tick = 0U;

  task_destroy(self);
}
