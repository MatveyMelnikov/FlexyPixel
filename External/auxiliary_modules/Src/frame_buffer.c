#include "frame_buffer.h"
#include "flash_driver.h"
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
static uint8_t erased_frame_index = 0;
static uint8_t amount_of_erased_sectors = 0;
static bool lock = false;
//static uint8_t amount_of_erased_sectors = 0;

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

// __attribute__((always_inline))
// inline static flash_driver_status get_bytes_in_frame(
//   const uint16_t frame_offset,
//   uint16_t *const pixels_in_frame
// )
// {
//   flash_driver_status status = flash_driver_read(
//     frame_offset + 30,
//     (uint8_t*)pixels_in_frame + 1, // Correction for byte sequence in uint16
//     sizeof(uint8_t)
//   );
//   // status |= eeprom_random_byte_read( // Invert?
//   //   frame_offset + 31,
//   //   (uint8_t*)pixels_in_frame + 1
//   // );

//   return status;
// }

// __attribute__((always_inline))
// inline 
static frame_buffer_status clear_pages_sectors()
{
  //uint8_t input_data;
  //uint16_t end_address = address + (page_size - 1);

  // flash_driver_status status = flash_driver_read(
  //   address,
  //   &input_data,
  //   sizeof(uint8_t)
  // );

  // if (input_data != 0xff)
  // {
  //   status |= flash_driver_sector_erase(address);
  //   return status | FRAME_BUFFER_IN_PROGRESS;
  // }
  
  // status |= flash_driver_read(
  //   end_address,
  //   &input_data,
  //   sizeof(uint8_t)
  // );
  // if (input_data != 0xff)
  // {
  //   status |= flash_driver_sector_erase(end_address);
  //   return status | FRAME_BUFFER_IN_PROGRESS;
  // }

  // return FRAME_BUFFER_OK;

  if (erased_frame_index >= frames_num)
    return FRAME_BUFFER_OK;

  flash_driver_status status = FLASH_DRIVER_OK;
  uint16_t erased_page_addr = erased_frame_index * 1024U;
  uint16_t num_of_start_data_sector = erased_page_addr / MEMORY_SECTOR_SIZE;
  if (num_of_start_data_sector >= amount_of_erased_sectors)
  {
    status |= flash_driver_sector_erase(
      num_of_start_data_sector * MEMORY_SECTOR_SIZE
    );
    amount_of_erased_sectors++;
    goto erase;
  }

  uint16_t num_of_end_data_sector = (erased_page_addr + sizeof(frame_buffer)) /
    MEMORY_SECTOR_SIZE;
  if (num_of_end_data_sector >= amount_of_erased_sectors)
  {
    status |= flash_driver_sector_erase(
      num_of_end_data_sector * MEMORY_SECTOR_SIZE
    );
    amount_of_erased_sectors++;
  }

erase:
  erased_frame_index++;
  return status | FRAME_BUFFER_IN_PROGRESS;
}

// Implementations -----------------------------------------------------------

void frame_buffer_set_frames_num(uint8_t num)
{
  frames_num = num;
}

void frame_buffer_set(const uint8_t *data)
{
  //memset(frame_buffer + 32, 0x0, sizeof(frame_buffer) - 32);
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

  if (pixels_num)
    __asm("nop");
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
  uint32_t write_address = page_addr + 1024U * frame_index;
  //uint16_t write_size = frame_buffer + MEMORY_PAGE_SIZE * page_num;

  frame_buffer_status erase_status = clear_pages_sectors();
  if (erase_status)
    return erase_status;

  // uint16_t page_size = sizeof(frame_buffer) - MEMORY_PAGE_SIZE * page_num;
  // if (page_size > MEMORY_PAGE_SIZE)
  //   page_size = MEMORY_PAGE_SIZE;

  uint16_t frame_offset = MEMORY_PAGE_SIZE * page_num;
  flash_driver_status status = flash_driver_write(
    write_address,
    frame_buffer + frame_offset,
    MEMORY_PAGE_SIZE
  );

  // while (flash_driver_is_busy()) {}

  // status = flash_driver_read(
  //   write_address,
  //   frame_buffer + frame_offset,
  //   MEMORY_PAGE_SIZE
  // );

  // while (flash_driver_is_busy()) {}

  if (status)
  {
    __asm("nop");
    return FRAME_BUFFER_ERROR;
  }

  uint8_t pages_border = sizeof(frame_buffer) / MEMORY_PAGE_SIZE;
  if (++page_num > pages_border)
  {
    // flash_driver_status status = flash_driver_write(
    //   write_address,
    //   frame_buffer + MEMORY_PAGE_SIZE * page_num,
    //   sizeof(frame_buffer) % MEMORY_PAGE_SIZE
    // );

    page_num = 0;
    page_addr = 0;
    if (++frame_index >= frames_num)
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
  if (loaded_frame >= frames_num)
    loaded_frame = 0;

  // flash_driver_status status = get_bytes_in_frame(
  //   loaded_addr,
  //   &frame_size
  // );
  flash_driver_status status = flash_driver_read(
    loaded_addr + 30,
    (uint8_t*)&frame_size,
    sizeof(uint16_t)
  );

  if (status)
    goto error;
  if (frame_size != sizeof(frame_buffer))
    goto error;

  // eeprom_addr_write(loaded_addr);
  // eeprom_sequential_read(frame_buffer, sizeof(frame_buffer));

  status = flash_driver_read(
    loaded_addr,
    frame_buffer,
    sizeof(frame_buffer)
  );
  if (status)
    goto error;

  loaded_addr += 1024U;
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
  frame_buffer_reset();
  displays_conf_clear();

  uint8_t frame_start = 0;
  uint16_t frame_size = 0;
  flash_driver_status status = flash_driver_read(0, &frame_start, 1);
  //status = eeprom_random_byte_read(0, &frame_start);
  if (status || frame_start != 0xaa)
    return;

  //status = get_bytes_in_frame(loaded_addr, &frame_size);
  status = flash_driver_read(30, (uint8_t*)&frame_size, sizeof(uint16_t));
  if (frame_size != sizeof(frame_buffer) || status)
    return;

  // status = eeprom_addr_write(20);
  // status |= eeprom_sequential_read((uint8_t*)&frames_num, 2);
  
  status = flash_driver_read(20, (uint8_t*)&frames_num, 1);
  if (frames_num == 0 || status)
    return;

  // Load configuration in frame
  uint8_t display_side_size = 0;
  led_panels_size configuration[CONFIGURATION_SIZE];
  uint8_t display_index = 0;
  for (; display_index < CONFIGURATION_SIZE; display_index++)
  {
    //status = eeprom_random_byte_read(21 + display_index, &display_side_size);
    status = flash_driver_read(21 + display_index, &display_side_size, 1);
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
  erased_frame_index = 0;
  amount_of_erased_sectors = 0;
  lock = false;
}
