#include "displays_config_storage.h"
#include "displays_config_repo.h"
#include "cy15b104q_driver.h"

// Static variables ----------------------------------------------------------

// Static prototypes ---------------------------------------------------------

static displays_config_repo_status displays_config_storage_save(
  const displays_config_repo_internal_data *const data
);
static displays_config_repo_status displays_config_storage_load(
  displays_config_repo_internal_data *const data
);

// Implementations -----------------------------------------------------------

void displays_config_storage_create()
{
  displays_config_repo_create(
    (displays_config_repo_io) {
      .load = displays_config_storage_load,
      .save = displays_config_storage_save
    }
  );
}

void displays_config_storage_destroy()
{
  displays_config_repo_destroy();
}

static displays_config_repo_status displays_config_storage_save(
  const displays_config_repo_internal_data *const data
)
{
  cy15b104q_driver_write_enable();
  cy15b104q_driver_status status = cy15b104q_driver_write_memory_data(
    (cy15b104q_driver_address) {
      .full = DISPLAYS_CONFIG_STORAGE_ADDRESS
    },
    (uint8_t*)data,
    sizeof(displays_config_repo_internal_data)
  );

  return (displays_config_repo_status)status;
}

static displays_config_repo_status displays_config_storage_load(
  displays_config_repo_internal_data *const data
)
{
  cy15b104q_driver_status status = cy15b104q_driver_read_memory_data(
    (cy15b104q_driver_address) {
      .full = DISPLAYS_CONFIG_STORAGE_ADDRESS
    },
    (uint8_t*)data,
    sizeof(displays_config_repo_internal_data)
  );

  return (displays_config_repo_status)status;
}