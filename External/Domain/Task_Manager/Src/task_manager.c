#include "task_manager.h"
#include "hash_table.h"
#include <stddef.h>

// Static variables ----------------------------------------------------------

static task_manager_io manager_io;
static hash_table clusters_table;
static hash_table_item clusters_space[TASK_MANAGER_CLUSTER_MAX_AMOUNT];
static uint8_t clusters_shared_buffer[TASK_MANAGER_SHARED_BUFFER_SIZE];
static task_data clusters_shared_output;
static uint32_t task_clusters_amount;
static uint32_t task_cluster_index;

// Static prototypes ---------------------------------------------------------

static task_manager_status task_manager_fill_hash_table(
  task_cluster *clusters,
  uint32_t clusters_amount
);
static bool task_manager_find_cluster(task_cluster **out, char *name);
static bool task_manager_is_cluster_empty(task_cluster *cluster);

// Implementations -----------------------------------------------------------

task_manager_status task_manager_create(
  task_manager_io io,
  task_cluster *clusters,
  uint32_t clusters_amount
)
{
  manager_io = io;

  if (task_manager_fill_hash_table(clusters, clusters_amount))
    return TASK_MANAGER_STATUS_ERROR;

  task_clusters_amount = clusters_amount;
  task_cluster_index = 0U;
  clusters_shared_output = (task_data) {
    .data = clusters_shared_buffer,
    .data_size = sizeof(clusters_shared_buffer),
    .is_changed = false
  };

  return TASK_MANAGER_STATUS_OK;
}

void task_manager_destroy()
{
  hash_table_destroy(&clusters_table);
  task_clusters_amount = 0U;
  task_cluster_index = 0U;
  clusters_shared_output = (task_data) { 0 };
}

static task_manager_status task_manager_fill_hash_table(
  task_cluster *clusters,
  uint32_t clusters_amount
)
{
  hash_table_status status = HASH_TABLE_STATUS_OK;

  clusters_table = hash_table_create(
    (uint8_t*)clusters_space,
    TASK_MANAGER_CLUSTER_MAX_AMOUNT * sizeof(hash_table_item)
  );

  for (
    uint8_t cluster_index = 0;
    cluster_index < clusters_amount;
    cluster_index++
  )
  {
    status |= hash_table_insert(
      &clusters_table,
      (hash_table_item) {
        .key = clusters[cluster_index].name,
        .ptr = &clusters[cluster_index]
      }
    );
  }

  return (status) ? TASK_MANAGER_STATUS_ERROR : TASK_MANAGER_STATUS_OK;
}

void task_manager_start_cluster(char *const name, task_data *const input)
{
  task_cluster *found_cluster = NULL;
  if (!task_manager_find_cluster(&found_cluster, name))
    return;

  task_cluster_start(
    found_cluster,
    (task_arg) {
      .input = input,
      .output = &clusters_shared_output,
      .current_tick = manager_io.get_tick()
    }
  );
}

void task_manager_stop_cluster(char *const name)
{
  task_cluster *found_cluster = NULL;
  if (!task_manager_find_cluster(&found_cluster, name))
    return;

  task_cluster_stop(found_cluster);
}

static bool task_manager_find_cluster(task_cluster **out, char *name)
{
  hash_table_item *found = hash_table_get(
    &clusters_table,
    name
  );

  if (found == NULL)
    return false;

  *out = found->ptr;
  return true;
}


void task_manager_execute()
{
  hash_table_item cluster_item;

  for (
    uint8_t cluster_index = 0;
    cluster_index < TASK_MANAGER_CLUSTER_MAX_AMOUNT;
    cluster_index++
  )
  {
    cluster_item = clusters_space[cluster_index];

    if (task_manager_is_cluster_empty(cluster_item.ptr))
      continue;

    (void)task_cluster_execute(
      (task_cluster*)cluster_item.ptr,
      manager_io.get_tick()
    );
    
    // Handle output
  }
}

static bool task_manager_is_cluster_empty(task_cluster *cluster)
{
  if (cluster == NULL)
    return true;
  return false;
}
