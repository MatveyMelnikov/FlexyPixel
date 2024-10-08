#ifndef FLASH_DRIVER_DEFS_H
#define FLASH_DRIVER_DEFS_H

enum
{
  FLASH_DRIVER_TIMEOUT = 0x100U,
  FLASH_DRIVER_PAGE_SIZE = 0x100U,
  FLASH_DRIVER_DUMMY_BYTE = 0x00U,
  FLASH_DRIVER_MANUFACTURER_ID = 0xefU,
  FLASH_DRIVER_SIZE = 0x1000000, // 16 mbyte
  FLASH_DRIVER_DEVICE_ID = 0x17U,
  FLASH_DRIVER_ID_CMD = 0x90U,
  FLASH_DRIVER_WRITE_ENABLE_CMD = 0x06U,
  FLASH_DRIVER_ERASE_SECTOR_CMD = 0x20U,
  FLASH_DRIVER_PAGE_PROGRAM_CMD = 0x02U,
  FLASH_DRIVER_READ_CMD = 0x03U,
  FLASH_DRIVER_GET_STATUS_1_CMD = 0x05U,
  FLASH_DRIVER_BUSY_BIT = 0x01U
};

typedef enum
{
  FLASH_DRIVER_OK = 0x00U,
  FLASH_DRIVER_ERROR = 0x01U,
  FLASH_DRIVER_BUSY = 0x02U,
  FLASH_DRIVER_TIMEOUT_ERROR = 0x03U,
  FLASH_DRIVER_OVERFLOW = 0x04U,
  FLASH_DRIVER_INVALID_ARGS = 0x05U
} flash_driver_status;

#endif
