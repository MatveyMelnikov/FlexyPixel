#include "displays_config_repo.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

// Static variables ----------------------------------------------------------

static displays_config_repo_io module_io;
static bool is_load_needed;
static displays_config_repo_internal_data local_data;
static displays_config_repo_external_data external_data;

// Static prototypes ---------------------------------------------------------

static bool displays_config_repo_is_start_byte_invalid(void);
static void displays_config_repo_reset(void);
static displays_config_repo_status displays_config_repo_load(void);
static void displays_config_repo_fill_external_data(void);
static void displays_config_repo_calculate_displays_amount(void);

// Implementations -----------------------------------------------------------

void displays_config_repo_create(displays_config_repo_io io)
{
  module_io = io;

  is_load_needed = true;
  local_data = (displays_config_repo_internal_data) { 0 };
  external_data = (displays_config_repo_external_data) { 0 };
}

void displays_config_repo_destroy()
{
  module_io = (displays_config_repo_io) { 0 };
}

const displays_config_repo_external_data *const displays_config_repo_get()
{
  if (displays_config_repo_load())
    return NULL;

  return &external_data;
}

static displays_config_repo_status displays_config_repo_load()
{
  if (!is_load_needed)
    return DISPLAYS_CONFIG_REPO_OK;

  displays_config_repo_status status = module_io.load(&local_data);
  is_load_needed = false;

  if (status)
    return status;
  if (displays_config_repo_is_start_byte_invalid())
    return DISPLAYS_CONFIG_REPO_INVALID;
  
  displays_config_repo_fill_external_data();
  is_load_needed = false;

  return DISPLAYS_CONFIG_REPO_OK;
}

static bool displays_config_repo_is_start_byte_invalid()
{
  if (local_data.start_byte == DISPLAYS_CONFIG_REPO_START_BYTE)
    return false;

  displays_config_repo_reset();
  return true;
}

static void displays_config_repo_reset()
{
  local_data = (displays_config_repo_internal_data) { 0 };
  external_data = (displays_config_repo_external_data) { 0 };
}

displays_config_repo_status displays_config_repo_save(
  const displays_config_repo_external_data *const new_config
)
{
  local_data = (displays_config_repo_internal_data) {
    .start_byte = DISPLAYS_CONFIG_REPO_START_BYTE,
    .frames_amount = new_config->frames_amount,
    .render_delay = new_config->render_delay
  };

  for (
    uint8_t display_index = 0;
    display_index < DISPLAYS_CONFIG_REPO_DISPLAYS_MAX_AMOUNT;
    display_index++
  )
  {
    local_data.configuration[display_index] =
      (uint8_t)new_config->configuration[display_index];
  }
  
  displays_config_repo_fill_external_data();

  return module_io.save(&local_data);
}

static void displays_config_repo_fill_external_data()
{
  external_data = (displays_config_repo_external_data) {
    .frames_amount = local_data.frames_amount,
    .render_delay = local_data.render_delay
  };

  for (
    uint8_t display_index = 0;
    display_index < DISPLAYS_CONFIG_REPO_DISPLAYS_MAX_AMOUNT;
    display_index++
  )
  {
    external_data.configuration[display_index] =
      (displays_config_repo_size)local_data.configuration[display_index];
  }

  displays_config_repo_calculate_displays_amount();
}

static void displays_config_repo_calculate_displays_amount()
{
  external_data.displays_amount = 0U;

  for (
    uint8_t display_index = 0;
    display_index < DISPLAYS_CONFIG_REPO_DISPLAYS_MAX_AMOUNT;
    display_index++
  )
  {
    if (external_data.configuration[display_index] == 0)
      break;

    external_data.displays_amount++;
  }
}
