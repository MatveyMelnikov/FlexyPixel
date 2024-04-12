#include "flash_driver.h"
#include "flash_driver_io.h"
#include <memory.h>
#include <stdlib.h>

// Defines -------------------------------------------------------------------

// Static functions ----------------------------------------------------------

__attribute__((always_inline))
inline static uint32_t flash_driver_reverse_addr(const uint32_t address)
{
  uint32_t result = address << 16;
  result |= address & 0xff00;
  result |= (address & 0xff0000) >> 16;

  return result;
}

__attribute__((always_inline))
inline static uint32_t flash_driver_is_write_args_wrong(
  const uint32_t address,
  const uint16_t data_size
)
{
  return address >= FLASH_DRIVER_SIZE || data_size > FLASH_DRIVER_PAGE_SIZE ||
    (address + data_size) > FLASH_DRIVER_SIZE;
}

// The Write Enable instruction sets the Write Enable Latch bit
// The WEL bit must be set prior to every Program and Erase command.
// Winbond W25Q128JV datasheet (pg. 24)
static flash_driver_status flash_driver_write_enable()
{
  uint8_t output_data = FLASH_DRIVER_WRITE_ENABLE_CMD;

  flash_driver_select(true);
  flash_driver_status status = flash_driver_io_write(
    &output_data, sizeof(uint8_t)
  );
  flash_driver_select(false);

  return status;
}

// Implementations -----------------------------------------------------------

flash_driver_status flash_driver_is_busy()
{
  uint8_t output_data[2] = {
    FLASH_DRIVER_GET_STATUS_1_CMD,
    FLASH_DRIVER_DUMMY_BYTE
  };
  uint8_t input_data[2];

  flash_driver_select(true);
  flash_driver_status status = flash_driver_io_write_read(
    output_data, input_data, sizeof(output_data)
  );
  flash_driver_select(false);

  if (status)
    return status;
  
  return input_data[1] & FLASH_DRIVER_BUSY_BIT ?
    FLASH_DRIVER_BUSY : FLASH_DRIVER_OK;
}

// Winbond W25Q128JV datasheet (pg. 23) - Manufacturer/Device ID cmd = 90h
flash_driver_status flash_driver_check_link()
{
  uint8_t output_data[6] = {
    FLASH_DRIVER_ID_CMD,
    FLASH_DRIVER_DUMMY_BYTE,
    FLASH_DRIVER_DUMMY_BYTE,
    0x0,
    FLASH_DRIVER_DUMMY_BYTE,
    FLASH_DRIVER_DUMMY_BYTE
  };
  uint8_t input_data[6];


  flash_driver_status status = flash_driver_is_busy();
  if (status)
    return status;
  
  flash_driver_select(true);
  status = flash_driver_io_write_read(
    output_data, input_data, sizeof(output_data)
  );
  flash_driver_select(false);

  if (status)
    return status;
  if (input_data[4] == FLASH_DRIVER_MANUFACTURER_ID &&
    input_data[5] == FLASH_DRIVER_DEVICE_ID)
    return FLASH_DRIVER_OK;
  else
    return FLASH_DRIVER_ERROR;
}

// The Page Program instruction allows from one byte to 256 bytes (a page)
// of data to be programmed at previously erased (FFh) memory locations.
// Winbond W25Q128JV datasheet (pg. 36)
flash_driver_status flash_driver_write(
  const uint32_t address,
  const uint8_t *const data,
  const uint16_t data_size
)
{
  if (data_size == 0 || flash_driver_is_write_args_wrong(address, data_size))
    return FLASH_DRIVER_INVALID_ARGS;

  flash_driver_status status = flash_driver_is_busy();
  status |= flash_driver_write_enable();
  if (status)
    return status;

  uint8_t *output_data = malloc(4 + data_size);
  uint32_t reversed_addr = flash_driver_reverse_addr(address);
  output_data[0] = FLASH_DRIVER_PAGE_PROGRAM_CMD;
  memcpy(output_data + 1, (uint8_t*)&reversed_addr, 3);
  memcpy(output_data + 4, data, data_size);

  flash_driver_select(true);
  status = flash_driver_io_write(
    output_data, 4 + data_size
  );
  flash_driver_select(false);

  free(output_data);

  return status;
}

// The Read Data instruction allows one or more data bytes to be sequentially
// read from the memory. the entire memory can be accessed with a single 
// instruction as long as the clock continues.
// Winbond W25Q128JV datasheet (pg. 28)
flash_driver_status flash_driver_read(
  uint32_t address,
  uint8_t *data,
  uint16_t data_size
)
{
  if (data_size == 0)
    return FLASH_DRIVER_INVALID_ARGS;
  flash_driver_status status = flash_driver_is_busy();
  if (status)
    return status;

  uint8_t *output_data = malloc(4 + data_size);
  uint8_t *input_data = malloc(4 + data_size);

  output_data[0] = FLASH_DRIVER_READ_CMD;
  uint32_t reversed_addr = flash_driver_reverse_addr(address);
  memcpy(output_data + 1, (uint8_t*)&reversed_addr, 3);

  flash_driver_select(true);
  status = flash_driver_io_write_read(
    output_data, input_data, 4 + data_size
  );
  flash_driver_select(false);

  if (status == FLASH_DRIVER_OK)
    memcpy(data, input_data + 4, data_size);

  free(output_data);
  free(input_data);
  return status;
}

// The Sector Erase instruction sets all memory within a specified sector
// (4K-bytes) to the erased state of all 1s (FFh).
// Winbond W25Q128JV datasheet (pg. 39)
flash_driver_status flash_driver_sector_erase(uint32_t address)
{
  flash_driver_status status = flash_driver_is_busy();
  status |= flash_driver_write_enable();
  if (status)
    return status;

  uint8_t output_data[4] = { FLASH_DRIVER_ERASE_SECTOR_CMD };
  // Get page address
  uint32_t reversed_addr = flash_driver_reverse_addr(address) & 0xfff000;
  memcpy(output_data + 1, (uint8_t*)&reversed_addr, 3);

  flash_driver_select(true);
  status = flash_driver_io_write(
    output_data, sizeof(output_data)
  );
  flash_driver_select(false);

  return status;
}
