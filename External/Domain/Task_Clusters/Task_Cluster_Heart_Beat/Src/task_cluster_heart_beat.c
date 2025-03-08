#include "task.h"
#include "task_cluster_heart_beat.h"
#include <stdlib.h>
#include <string.h>

// Defines -------------------------------------------------------------------

// Static variables ----------------------------------------------------------

static task_cluster_heart_beat_io cluster_io;
static task_interface_struct interface_init;
static task_interface_struct interface_heart_beat;
static uint32_t start_tick = 0U;

// Static functions ----------------------------------------------------------

static task_output task_heart_beat_init(task_arg *const argument);
static task_output task_heart_beat(task_arg *const argument);

// Implementations -----------------------------------------------------------

task_cluster task_cluster_heart_beat_create(task_cluster_heart_beat_io io)
{
  cluster_io = io;

  interface_init = (task_interface_struct) {
    .run = task_heart_beat_init,
    .destroy = task_destroy
  };
  interface_heart_beat = (task_interface_struct) {
    .run = task_heart_beat,
    .destroy = task_destroy
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
      .vtable = &interface_heart_beat
    }
  };

  return (task_cluster) {
    .name = TASK_CLUSTER_HEART_BEAT_NAME,
    .tasks = cluster_tasks,
    .tasks_amount = (sizeof(cluster_tasks) / sizeof(task))
  };
}

static task_output task_heart_beat_init(task_arg *const argument)
{
  start_tick = argument->current_tick;

  return (task_output) {
    .response = TASK_RESPONSE_OK,
    .status = TASK_STATUS_COMPLETED
  };
}

static task_output task_heart_beat(task_arg *const argument)
{

  if ((argument->current_tick - start_tick) < TASK_CLUSTER_HEART_BEAT_DELAY)
  {
    return (task_output) {
      .response = TASK_RESPONSE_OK,
      .status = TASK_STATUS_IN_PROGRESS
    };
  }

  start_tick = argument->current_tick;
  cluster_io.toggle_led();

  return (task_output) {
    .response = TASK_RESPONSE_OK,
    .status = TASK_STATUS_IN_PROGRESS
  };
}
