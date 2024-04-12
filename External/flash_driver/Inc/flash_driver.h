#ifndef FLASH_DRIVER_H
#define FLASH_DRIVER_H

#include "flash_driver_defs.h"
#include <stdint.h>

flash_driver_status flash_driver_is_busy();
flash_driver_status flash_driver_check_link();
flash_driver_status flash_driver_write(
  const uint32_t address,
  const uint8_t *const data,
  const uint16_t data_size
);
flash_driver_status flash_driver_read(
  const uint32_t address,
  uint8_t *const data,
  const uint16_t data_size
);
flash_driver_status flash_driver_sector_erase(uint32_t address);

#endif
