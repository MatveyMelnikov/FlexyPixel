#include "list_of_changes.h"
#include <led_panels_driver.h>
#include <displays_conf.h>
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>

// Static variables ----------------------------------------------------------

enum {
  CHANGES_SIZE = 576U, // 9 panels (8 x 8)
  PANELS_NUM = 9U
};

typedef struct {
  uint8_t panel_index;
  uint8_t pixel_index;
  uint16_t color; // 0r, gb
} pixel_change;

static pixel_change changes[CHANGES_SIZE];
static uint16_t changes_top = 0;
static bool is_updated = false;
static bool are_there_any_changes = false;
static led_panels_size sizes[PANELS_NUM] = {
  LED_PANELS_SIZE_64,
  LED_PANELS_SIZE_64,
  LED_PANELS_SIZE_64,
  LED_PANELS_SIZE_64,
  LED_PANELS_SIZE_64,
  LED_PANELS_SIZE_64,
  LED_PANELS_SIZE_64,
  LED_PANELS_SIZE_64,
  LED_PANELS_SIZE_64
};

// Defines -------------------------------------------------------------------

#define PACK_COLOR(color) \
  ((uint16_t)((color).red) << 8) | \
  ((uint16_t)((color).green) << 4) | \
  (uint16_t)((color).blue)

#define UNPACK_COLOR(color) \
  (led_panels_color) { \
    .red = ((color) & 0x0f00) >> 8, \
    .green = ((color) & 0x00f0) >> 4, \
    .blue = ((color) & 0x000f) \
  }

// Static functions ----------------------------------------------------------

__attribute__((always_inline))
inline static bool is_colors_same(
  const led_panels_color *const first_color,
  const led_panels_color *const second_color
)
{
  return (
    first_color->red == second_color->red &&
    first_color->green == second_color->green &&
    first_color->blue == second_color->blue
  );
}

// Implementations -----------------------------------------------------------

bool list_of_changes_add(
  uint8_t panel_index,
  uint8_t pixel_index,
  led_panels_color color
)
{
  if (panel_index > PANELS_NUM)
    return false;
  if (pixel_index > sizes[panel_index])
    return false;

  changes[changes_top++] = (pixel_change) {
    .panel_index = panel_index,
    .pixel_index = pixel_index,
    .color = PACK_COLOR(color)
  };
  is_updated = true;
  are_there_any_changes = true;

  return true;
}

void list_of_changes_apply_changes(led_panels_buffer *const target)
{
  for (uint16_t change_index = 0; change_index < changes_top; change_index++)
  {
    pixel_change current_change = changes[change_index];
    uint8_t side_size = get_side_size(
      (uint16_t)sizes[current_change.panel_index]
    );

    led_panels_set_pixel(
      target,
      current_change.panel_index,
      current_change.pixel_index % side_size,
      current_change.pixel_index / side_size,
      UNPACK_COLOR(current_change.color)
    );
  }

  is_updated = false;
}

void list_of_changes_apply_raw_changes(
  uint8_t *const target,
  uint16_t target_len
)
{
  led_panels_buffer projection = (led_panels_buffer) {
    .is_locking = false,
    .panels_num = displays_conf_get_displays_num(),
    .panels_sizes = displays_conf_get(),
    .pixel_data = target,
    .pixel_data_size = target_len,
    .pwm_data = NULL,
    .transmit_index = 0
  };

  list_of_changes_apply_changes(&projection);
}

bool list_of_changes_is_updated()
{
  return is_updated;
}

bool list_of_changes_are_there_any_changes()
{
  return are_there_any_changes;
}

void list_of_changes_clear()
{
  changes_top = 0;
  is_updated = false;
  are_there_any_changes = false;
}
