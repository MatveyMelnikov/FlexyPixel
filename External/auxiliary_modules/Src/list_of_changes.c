#include "list_of_changes.h"
#include "flash_driver.h"
#include "displays_conf.h"
#include "led_panels_driver.h"
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>

// Static variables ----------------------------------------------------------

enum {
  CHANGES_SIZE = 576U, // 9 panels (8 x 8)
  PANELS_NUM = 9U,
  CHANGES_ADDRESS = 0xfff000U, // last sector of last block
  HEADER_OFFSET = 0xffa // sixth byte from the end
};

typedef struct {
  uint8_t panel_index;
  uint8_t pixel_index;
  uint16_t color; // 0r, gb
} pixel_change;

static pixel_change changes[CHANGES_SIZE] = { 0 };
static uint16_t changes_top = 0;
static bool is_updated = false;
static bool is_need_to_save = false;
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
static bool save_progress[3] = { false, false, false };
static uint16_t save_addr_offset = 0;

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

#define RESET_SAVE_PROGRESS() \
  memset(save_progress, false, sizeof(save_progress)); \
  save_addr_offset = 0

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

__attribute__((always_inline))
inline static list_of_changes_status handle_save_operation(
  const flash_driver_status save_status,
  const uint8_t progress_stage_index
)
{
  switch (save_status)
  {
    case FLASH_DRIVER_OK:
      save_progress[progress_stage_index] = true;
      return LIST_OF_CHANGES_IN_PROGRESS;
    case FLASH_DRIVER_BUSY:
      return LIST_OF_CHANGES_IN_PROGRESS;
    default:
      return LIST_OF_CHANGES_ERROR;
  }
}

static pixel_change *find_change(uint8_t panel_index, uint8_t pixel_index)
{
  for (uint8_t change_index = 0; change_index < changes_top; change_index++)
  {
    pixel_change *current_change = &changes[change_index];
    if (
      current_change->panel_index == panel_index &&
      current_change->pixel_index == pixel_index
    )
      return current_change;
  }

  return NULL;
}

static list_of_changes_status reset_sector()
{
  if (save_progress[0])
    return LIST_OF_CHANGES_OK;

  return handle_save_operation(
    flash_driver_sector_erase(CHANGES_ADDRESS),
    0
  );
}

static list_of_changes_status save_start_bytes()
{
  if (save_progress[1])
    return LIST_OF_CHANGES_OK;

  uint16_t start_bytes[3] = { 0xaaaa, CHANGES_SIZE, changes_top };

  flash_driver_status status = flash_driver_write(
    CHANGES_ADDRESS + HEADER_OFFSET,
    (uint8_t*)start_bytes,
    sizeof(start_bytes)
  );

  return handle_save_operation(status, 1);
}

static list_of_changes_status save_changes()
{
  uint16_t data_size = sizeof(changes) < FLASH_DRIVER_PAGE_SIZE ? 
    sizeof(changes) : FLASH_DRIVER_PAGE_SIZE;

  flash_driver_status status = flash_driver_write(
    CHANGES_ADDRESS + save_addr_offset,
    (uint8_t*)changes + save_addr_offset,
    data_size
  );

  switch (status)
  {
    case FLASH_DRIVER_OK:
      save_addr_offset += FLASH_DRIVER_PAGE_SIZE;

      if (save_addr_offset > sizeof(changes))
        return LIST_OF_CHANGES_OK;
      else
        return LIST_OF_CHANGES_IN_PROGRESS;
    case FLASH_DRIVER_BUSY:
      return LIST_OF_CHANGES_IN_PROGRESS;
    default:
      return LIST_OF_CHANGES_ERROR;
  }
}

// Implementations -----------------------------------------------------------

list_of_changes_status list_of_changes_load()
{
  uint16_t header[3];

  flash_driver_status status = flash_driver_read(
    CHANGES_ADDRESS + HEADER_OFFSET,
    (uint8_t*)header,
    sizeof(header)
  );
  if (status || header[0] != 0xaaaa || header[1] != CHANGES_SIZE)
    return LIST_OF_CHANGES_ERROR;

  changes_top = header[2];

  status = flash_driver_read(
    CHANGES_ADDRESS,
    (uint8_t*)&changes,
    sizeof(changes)
  );

  if (status)
    return LIST_OF_CHANGES_ERROR;

  return LIST_OF_CHANGES_OK;
}

/**
 * Everything is stored in the last page (15) of the last block (255).
 * The header is written in the last 6 bytes to avoid address overflow
 * when writing changes at the beginning of the page.
*/
list_of_changes_status list_of_changes_save()
{
  list_of_changes_status status = reset_sector();
  if (status)
    return status;

  status = save_start_bytes();
  if (status)
    return status;

  status = save_changes();
  if (status)
    return status;

  RESET_SAVE_PROGRESS();

  return LIST_OF_CHANGES_OK;
}

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

  pixel_change *found_change = find_change(panel_index, pixel_index);
  if (found_change == NULL)
  {
    changes[changes_top++] = (pixel_change) {
      .panel_index = panel_index,
      .pixel_index = pixel_index,
      .color = PACK_COLOR(color)
    };
  }
  else
  {
    found_change->color = PACK_COLOR(color);
  }
  
  is_updated = true;
  is_need_to_save = true;

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

bool list_of_changes_is_need_to_save()
{
  return is_need_to_save;
}

void list_of_changes_clear()
{
  changes_top = 0;
  is_updated = false;
  is_need_to_save = false;
  RESET_SAVE_PROGRESS();
}
