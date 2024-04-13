#include "frame_buffer.h"
#include "flash_driver.h"
#include "displays_conf.h"
#include <memory.h>
#include <math.h>

// Defines -------------------------------------------------------------------

#define SECOND_PART(pixel_channel) \
  (((pixel_channel) - '0') & 0x0f)

#define PUT_UINT16(data_ptr, num) \
  *(uint16_t*)(data_ptr) = (num)

// Static variables ----------------------------------------------------------

static uint8_t frame_buffer[FRAME_BUFFER_SIZE]; // RGB
static uint16_t page_num = 0;
static uint32_t page_addr = 0;
static uint16_t frame_index = 0;
static uint16_t loaded_frame = 0;
static uint32_t loaded_addr = 0;
static uint16_t frames_amount = 0;
static uint16_t erased_frame_index = 0;
static bool lock = false;

// Static functions ----------------------------------------------------------

// pixels - pointer to first pixel
__attribute__((always_inline))
inline static void convert_pixels_to_data(
  const uint8_t *const pixels,
  uint16_t frame_index
)
{
  // r and g
  frame_buffer[frame_index] = (SECOND_PART(pixels[0]) << 4) |
    SECOND_PART(pixels[1]);
  // b and r
  frame_buffer[frame_index + 1] = (SECOND_PART(pixels[2]) << 4) |
    SECOND_PART(pixels[3]);
  // g anf b
  frame_buffer[frame_index + 2] = (SECOND_PART(pixels[4]) << 4) |
    SECOND_PART(pixels[5]);
}

__attribute__((always_inline))
inline static frame_buffer_status clear_sectors_with_pages()
{
  if (erased_frame_index >= frames_amount)
    return FRAME_BUFFER_OK;
  
  uint32_t sector_start = (erased_frame_index++) * FRAME_SIZE_IN_MEMORY;
  if (sector_start % MEMORY_SECTOR_SIZE != 0)
    return FRAME_BUFFER_IN_PROGRESS;

  flash_driver_status status = flash_driver_sector_erase(sector_start);

  return status | FRAME_BUFFER_IN_PROGRESS;
}

// Implementations -----------------------------------------------------------

void frame_buffer_set_frames_amount(uint16_t amount)
{
  frames_amount = amount;
}

void frame_buffer_set(const uint8_t *data)
{
  // memset(frame_buffer, 0xaa, 32); // start bytes
  memset(frame_buffer, 0xaa, 18); // start bytes
  PUT_UINT16(frame_buffer + 19, frames_amount);
  PUT_UINT16(frame_buffer + 30, sizeof(frame_buffer)); // only for 8x8

  // Set current configuration
  uint8_t *frame_conf = frame_buffer + 21;
  for (uint8_t i = 0; i < CONFIGURATION_SIZE; i++)
  {
    uint16_t display_size = displays_conf_get()[i];
    if (display_size == 0)
      *frame_conf = 0;
    else
      *frame_conf = get_side_size(displays_conf_get()[i]);
    frame_conf++;
  }

  uint16_t pixels_num = displays_conf_get_pixels_num() * 3;

  for (uint16_t data_index = 0; data_index < pixels_num; data_index += 6)
  {
    uint16_t frame_index = 32 + data_index / 2;
    convert_pixels_to_data(data + data_index, frame_index);
  }
}

void frame_buffer_lock(bool is_locked)
{
  lock = is_locked;
}

bool frame_buffer_is_locked()
{
  return lock;
}

bool frame_buffer_is_busy()
{
  return (flash_driver_is_busy() == FLASH_DRIVER_BUSY);
}

frame_buffer_status frame_buffer_save()
{
  uint32_t write_address = page_addr + FRAME_SIZE_IN_MEMORY * frame_index;

  frame_buffer_status erase_status = clear_sectors_with_pages();
  if (erase_status)
    return erase_status;

  uint16_t frame_offset = MEMORY_PAGE_SIZE * page_num;
  flash_driver_status status = flash_driver_write(
    write_address,
    frame_buffer + frame_offset,
    MEMORY_PAGE_SIZE
  );

  if (status)
    return FRAME_BUFFER_ERROR;

  uint8_t pages_border = sizeof(frame_buffer) / MEMORY_PAGE_SIZE;
  if (++page_num > pages_border)
  {
    page_num = 0;
    page_addr = 0;
    if (++frame_index >= frames_amount)
      frame_index = 0;
    return FRAME_BUFFER_OK;
  }

  page_addr += MEMORY_PAGE_SIZE;
  return FRAME_BUFFER_IN_PROGRESS;
}

frame_buffer_status frame_buffer_load(led_panels_buffer *const buffer)
{
  uint16_t frame_size = 0;

  if (lock)
    return FRAME_BUFFER_LOCK;
  if (loaded_frame >= frames_amount)
  {
    loaded_addr = 0;
    loaded_frame = 0;
  }

  flash_driver_status status = flash_driver_read(
    loaded_addr + 30,
    (uint8_t*)&frame_size,
    sizeof(uint16_t)
  );

  if (status)
    goto error;
  if (frame_size != sizeof(frame_buffer))
    goto error;

  status = flash_driver_read(
    loaded_addr,
    frame_buffer,
    sizeof(frame_buffer)
  );
  if (status)
    goto error;

  loaded_addr += FRAME_SIZE_IN_MEMORY;
  loaded_frame++;

  memcpy(
    buffer->pixel_data,
    frame_buffer + 32,
    buffer->pixel_data_size
  );

  return FRAME_BUFFER_OK;

error:
  loaded_addr = 0;
  loaded_frame = 0;
  return FRAME_BUFFER_ERROR;
}

void frame_buffer_load_conf()
{
  uint8_t frame_start = 0;
  uint16_t frame_size = 0;

  frame_buffer_reset();
  displays_conf_clear();

  flash_driver_status status = flash_driver_read(0, &frame_start, 1);
  if (status || frame_start != 0xaa)
    return;

  status = flash_driver_read(30, (uint8_t*)&frame_size, sizeof(uint16_t));
  if (frame_size != sizeof(frame_buffer) || status)
    return;
  
  status = flash_driver_read(19, (uint8_t*)&frames_amount, sizeof(uint16_t));
  if (frames_amount == 0 || status)
    return;

  // Load configuration of frame
  led_panels_size configuration[CONFIGURATION_SIZE];
  uint8_t loaded_configuration[9] = { 0 };
  uint8_t display_index = 0;

  status = flash_driver_read(
    21,
    loaded_configuration,
    sizeof(loaded_configuration)
  );
  if (status)
    return status;

  for (; display_index < CONFIGURATION_SIZE; display_index++)
  {
    uint8_t display_side_size = loaded_configuration[display_index];
    if (display_side_size == 0)
      break;
    configuration[display_index] = (uint16_t)powf(display_side_size, 2);
  }

  displays_conf_update(configuration, display_index);
}

void frame_buffer_reset()
{
  page_num = 0;
  page_addr = 0;
  loaded_frame = 0;
  frame_index = 0;
  erased_frame_index = 0;
  lock = false;
}
