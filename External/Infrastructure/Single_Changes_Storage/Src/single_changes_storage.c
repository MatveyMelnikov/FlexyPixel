#include "single_changes_storage.h"
#include "single_changes_repo.h"
#include "cy15b104q_driver.h"

// Static variables ----------------------------------------------------------

// Static prototypes ---------------------------------------------------------

static single_changes_repo_status single_changes_storage_save(
  const single_changes_repo_internal_data *const data
);
static single_changes_repo_status single_changes_storage_load(
  single_changes_repo_internal_data *const data
);

// Implementations -----------------------------------------------------------

void single_changes_storage_create()
{
  single_changes_repo_create(
    (single_changes_repo_io) {
      .load = single_changes_storage_load,
      .save = single_changes_storage_save
    }
  );
}

void single_changes_storage_destroy()
{
  single_changes_repo_destroy();
}

static single_changes_repo_status single_changes_storage_save(
  const single_changes_repo_internal_data *const data
)
{
  cy15b104q_driver_write_enable();
  cy15b104q_driver_status status = cy15b104q_driver_write_memory_data(
    (cy15b104q_driver_address) {
      .full = SINGLE_CHANGES_STORAGE_ADDRESS
    },
    (uint8_t*)data,
    sizeof(single_changes_repo_internal_data)
  );

  return (cy15b104q_driver_status)status;
}

static single_changes_repo_status single_changes_storage_load(
  single_changes_repo_internal_data *const data
)
{
  cy15b104q_driver_status status = cy15b104q_driver_read_memory_data(
    (cy15b104q_driver_address) {
      .full = SINGLE_CHANGES_STORAGE_ADDRESS
    },
    (uint8_t*)data,
    sizeof(single_changes_repo_internal_data)
  );

  return (cy15b104q_driver_status)status;
}
