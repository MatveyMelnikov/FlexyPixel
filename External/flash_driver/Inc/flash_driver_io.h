#ifndef FLASH_DRIVER_IO_H
#define FLASH_DRIVER_IO_H

#include "flash_driver_defs.h"
#include <stdint.h>
#include <stdbool.h>

flash_driver_status flash_driver_io_write(
  uint8_t *const output_data,
  const uint16_t output_data_size
);
flash_driver_status flash_driver_io_write_read(
  uint8_t *const output_data,
  uint8_t *const input_data,
  const uint16_t output_data_size
);
void flash_driver_select(bool select);

#endif
