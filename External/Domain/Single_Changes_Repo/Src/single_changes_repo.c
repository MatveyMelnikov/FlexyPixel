#include "single_changes_repo.h"
#include <stddef.h>
#include <string.h>

// Static variables ----------------------------------------------------------

static single_changes_repo_io module_io;
static single_changes_repo_internal_data internal_data;
static single_changes_repo_external_data external_data;
static bool is_load_needed;
static bool is_save_needed;

// Static prototypes ---------------------------------------------------------

static single_changes_repo_status single_changes_repo_load(void);
static bool single_changes_repo_is_start_byte_invalid(void);
static void single_changes_repo_fill_external_data(void);
static bool single_changes_repo_is_out_of_bounds(
  uint8_t panel_index,
  uint8_t pixel_index
);
static void single_changes_repo_set_to_internal_data(
  uint8_t panel_index,
  uint8_t pixel_index,
  led_panels_color color
);
static void single_changes_repo_set_to_external_data(
  uint8_t panel_index,
  uint8_t pixel_index,
  led_panels_color color
);

// Implementations -----------------------------------------------------------

void single_changes_repo_create(single_changes_repo_io io)
{
  module_io = io;

  single_changes_repo_reset();
  is_load_needed = true;
  is_save_needed = false;
}

void single_changes_repo_destroy()
{
  module_io = (single_changes_repo_io) { 0 };
}

const single_changes_repo_external_data *const single_changes_repo_get()
{
  if (single_changes_repo_load())
    return NULL;

  return &external_data;
}

static single_changes_repo_status single_changes_repo_load()
{
  if (!is_load_needed)
    return SINGLE_CHANGES_REPO_OK;

  single_changes_repo_status status = module_io.load(&internal_data);

  if (status)
    return status;
  if (single_changes_repo_is_start_byte_invalid())
    return SINGLE_CHANGES_REPO_INVALID;

  single_changes_repo_fill_external_data();
  is_load_needed =  false;

  return SINGLE_CHANGES_REPO_OK;
}

static bool single_changes_repo_is_start_byte_invalid()
{
  if (internal_data.start_byte == SINGLE_CHANGES_REPO_START_BYTE)
    return false;

  single_changes_repo_reset();
  return true;
}

void single_changes_repo_reset()
{
  internal_data = (single_changes_repo_internal_data) { 0 };
  external_data = (single_changes_repo_external_data) { 0 };
  is_save_needed = true;
}

static void single_changes_repo_fill_external_data()
{
  single_changes_repo_external_change *linear_external_changes = 
    (single_changes_repo_external_change*)external_data.changes;

  for (
    uint16_t pixel_index = 0;
    pixel_index < SINGLE_CHANGES_REPO_CHANGES_AMOUNT;
    pixel_index++
  )
  {
    // rrrr ggggbbbb
    uint16_t internal_color = internal_data.changes[pixel_index].
      pixel_color;

    linear_external_changes[pixel_index] = 
      (single_changes_repo_external_change) {
        .color = (led_panels_color) {
          .red = (internal_color & 0xf00) >> 8,
          .green = (internal_color & 0xf0) >> 4,
          .blue = (internal_color & 0xf)
        },
        .is_active = (bool)internal_data.changes[pixel_index].is_active
      };
  }
}

void single_changes_repo_set( 
  uint8_t panel_index,
  uint8_t pixel_index,
  led_panels_color color
)
{
  if (single_changes_repo_is_out_of_bounds(panel_index, pixel_index))
    return;

  single_changes_repo_set_to_internal_data(panel_index, pixel_index, color);
  single_changes_repo_set_to_external_data(panel_index, pixel_index, color);

  is_save_needed = true;
}

static bool single_changes_repo_is_out_of_bounds(
  uint8_t panel_index,
  uint8_t pixel_index
)
{
  if (panel_index > SINGLE_CHANGES_REPO_PANELS_AMOUNT)
    return true;
  if (pixel_index > LED_PANELS_SIZE_64)
    return true;

  return false;
}

static void single_changes_repo_set_to_internal_data(
  uint8_t panel_index,
  uint8_t pixel_index,
  led_panels_color color
)
{
  // xxxxrrrr.ggggbbbb
  uint16_t internal_color = (((uint16_t)color.red & 0xf) << 8) |
    (((uint16_t)color.green & 0xf) << 4) | ((uint16_t)color.blue & 0xf);

  internal_data.changes[
    panel_index * LED_PANELS_SIZE_64 + pixel_index
  ] = (single_changes_repo_internal_change) {
    .pixel_color = internal_color,
    .is_active = (uint8_t)true
  };
}

static void single_changes_repo_set_to_external_data(
  uint8_t panel_index,
  uint8_t pixel_index,
  led_panels_color color
)
{
  external_data.changes[panel_index][pixel_index] = 
    (single_changes_repo_external_change) {
      .color = (led_panels_color) {
        .red = color.red,
        .green = color.green,
        .blue = color.blue
      },
      .is_active = true
    };
}

single_changes_repo_status single_changes_repo_save()
{
  if (!is_save_needed)
    return SINGLE_CHANGES_REPO_OK;

  internal_data.start_byte = SINGLE_CHANGES_REPO_START_BYTE;
  single_changes_repo_status status = module_io.save(&internal_data);

  if (status)
    return SINGLE_CHANGES_REPO_ERROR;

  is_save_needed = false;
  return status;
}
