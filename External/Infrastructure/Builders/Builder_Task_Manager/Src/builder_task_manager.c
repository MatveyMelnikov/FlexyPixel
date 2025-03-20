#include "builder_task_manager.h"
#include "task_cluster_receive_cmd.h"
#include "task_cluster_set_config.h"
#include "task_cluster_heart_beat.h"
#include "task_cluster_startup.h"
#include "task_cluster_render.h"
#include "task_cluster_set_mode.h"
#include "task_cluster_receive_data.h"
#include "task_cluster_pix_data.h"
#include "task_cluster_save.h"
#include "task_cluster_seq_params.h"
#include "task_cluster_seq_data.h"
#include <stdbool.h>
#include <string.h>

// Defines -------------------------------------------------------------------

#define CLUSTERS_AMOUNT 11U

// Static variables ----------------------------------------------------------

static task_cluster clusters[CLUSTERS_AMOUNT];

// Static prototypes ---------------------------------------------------------

static void load_all_clusters(void);
static void toggle_heart_beat_led(void);

// Implementations -----------------------------------------------------------

task_manager_status builder_task_manager_build()
{
  load_all_clusters();

  return task_manager_create(
    (task_manager_io) {
      .get_tick = HAL_GetTick
    },
    clusters,
    sizeof(clusters) / sizeof(task_cluster)
  );
}

static void load_all_clusters()
{
  task_cluster all_clusters[CLUSTERS_AMOUNT] = 
  {
    task_cluster_heart_beat_create(
      (task_cluster_heart_beat_io) {
        .toggle_led = toggle_heart_beat_led
      }
    ),
    task_cluster_request_cmd_create(
      (task_cluster_receive_cmd_io) {
        .start_cluster = task_manager_start_cluster
      }
    ),
    task_cluster_set_config_create(
      (task_cluster_set_config_io) {
        .start_cluster = task_manager_start_cluster
      }
    ),
    task_cluster_startup_create(
      (task_cluster_startuo_io) {
        .start_cluster = task_manager_start_cluster
      }
    ),
    task_cluster_render_create(),
    task_cluster_set_mode_create(
      (task_cluster_set_mode_io) {
        .start_cluster = task_manager_start_cluster
      }
    ),
    task_cluster_receive_data_create(
      (task_cluster_receive_data_io) {
        .start_cluster = task_manager_start_cluster
      }
    ),
    task_cluster_pix_data_create(
      (task_cluster_pix_data_io) {
        .start_cluster = task_manager_start_cluster
      }
    ),
    task_cluster_save_create(
      (task_cluster_save_io) {
        .start_cluster = task_manager_start_cluster
      }
    ),
    task_cluster_seq_params_create(
      (task_cluster_seq_params_io) {
        .start_cluster = task_manager_start_cluster
      }
    ),
    task_cluster_seq_data_create(
      (task_cluster_seq_data_io) {
        .start_cluster = task_manager_start_cluster
      }
    )
  };

  memcpy(clusters, all_clusters, sizeof(all_clusters));
}

static void toggle_heart_beat_led()
{
  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}

void builder_task_manager_destroy()
{
  task_manager_destroy();
}