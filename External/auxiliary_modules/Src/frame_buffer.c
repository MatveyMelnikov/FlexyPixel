#include "frame_buffer.h"
#include "eeprom_driver.h"
#include "displays_conf.h"
#include <memory.h>

// Defines -------------------------------------------------------------------

#define FIRST_PART(pixel_channel) \
  (((pixel_channel) - '0') & 0xf0)

#define SECOND_PART(pixel_channel) \
  (((pixel_channel) - '0') & 0x0f)

// Static variables ----------------------------------------------------------

static uint8_t frame_buffer[FRAME_BUFFER_SIZE]; // GRB
static uint16_t page_num = 0; // 512 pages in memory
static uint16_t page_addr = 0;
static uint16_t frame_index = 0;
static uint16_t loaded_frame = 0;
static uint16_t loaded_addr = 0;
static uint16_t frames_num = 0;

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
inline static eeprom_status get_bytes_in_frame(
  const uint16_t frame_offset,
  uint16_t *const pixels_in_frame
)
{
  eeprom_status status = eeprom_random_byte_read(
    frame_offset + 30,
    (uint8_t*)pixels_in_frame
  );
  status |= eeprom_random_byte_read( // Invert?
    frame_offset + 31,
    (uint8_t*)pixels_in_frame + 1
  );

  return status;
}

// Implementations -----------------------------------------------------------

void frame_buffer_set_frames_num(uint8_t num)
{
  frames_num = num;
}

void frame_buffer_set(const uint8_t *data)
{
  memset(frame_buffer, 0xaa, 32); // start bytes
  *(uint16_t*)(frame_buffer + 30) = 896; // only for 8x8
  *(frame_buffer + 20) = frames_num;

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

bool frame_buffer_is_saved()
{
  return eeprom_is_ready();
}

bool frame_buffer_save()
{
  eeprom_status status = eeprom_aligned_page_write(
    page_addr + sizeof(frame_buffer) * frame_index,
    frame_buffer + EEPROM_PAGE_SIZE * page_num
  );

  if (status)
  {
    __asm("nop");
    return false;
  }

  uint8_t pages_border = sizeof(frame_buffer) / 64; // 14 for 8x8
  if (++page_num >= pages_border)
  {
    page_num = 0;
    page_addr = 0;
    if (++frame_index >= frames_num)
      frame_index = 0;
    return false;
  }

  page_addr += 64;
  return true;
}

bool frame_buffer_load(led_panels_buffer *const buffer)
{
  uint16_t frame_size = 0;

  if (loaded_frame >= frames_num)
    loaded_frame = 0;

  eeprom_status status = get_bytes_in_frame(
    loaded_addr,
    &frame_size
  );

  if (status)
    goto error;
  if (frame_size != sizeof(frame_buffer))
    goto error;

  eeprom_addr_write(loaded_addr);
  eeprom_sequential_read(frame_buffer, sizeof(frame_buffer));

  loaded_addr += sizeof(frame_buffer);
  loaded_frame++;

  //memcpy(data, frame_buffer + 32, sizeof(frame_buffer) - 32);
  memcpy(
    buffer->pixel_data,
    frame_buffer + 32,
    buffer->pixel_data_size
  );

  return true;

error:
  loaded_addr = 0;
  loaded_frame = 0;
  return false;
}

void frame_buffer_load_conf()
{
  frame_buffer_reset();
  displays_conf_clear();

  uint8_t frame_start = 0;
  uint16_t frame_size = 0;
  eeprom_status status = EEPROM_OK;
  status = eeprom_random_byte_read(0, &frame_start);
  if (frame_start != 0xaa || status)
    return;

  status = get_bytes_in_frame(loaded_addr, &frame_size);
  if (frame_size != sizeof(frame_buffer) || status)
    return;

  status = eeprom_addr_write(20);
  status |= eeprom_sequential_read((uint8_t*)&frames_num, 2);
  //status = eeprom_random_byte_read(20, (uint8_t)&frames_num);
  if (frames_num == 0 || status)
    return;

  // Load configuration in frame
  uint8_t display_side_size = 0;
  led_panels_size configuration[CONFIGURATION_SIZE];
  uint8_t display_index = 0;
  for (; display_index < CONFIGURATION_SIZE; display_index++)
  {
    status = eeprom_random_byte_read(21 + display_index, &display_side_size);
    if (status)
      return;
    if (display_side_size == 0)
      break;
    
    configuration[display_index] = (uint16_t)display_side_size *
      (uint16_t)display_side_size;
  }

  displays_conf_update(configuration, display_index);
}

void frame_buffer_reset()
{
  page_num = 0;
  page_addr = 0;
  loaded_frame = 0;
  frame_index = 0;
}
