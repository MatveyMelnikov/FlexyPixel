#ifndef MOCK_FLASH_DRIVER_IO_H
#define MOCK_FLASH_DRIVER_IO_H

#include "flash_driver_io.h"
#include <stdint.h>

void mock_flash_driver_io_create(const uint8_t max_expectations);
void mock_flash_driver_io_destroy(void);
void mock_flash_driver_io_expect_write(
  const uint8_t *const output_data,
  const uint16_t output_data_size
);
void mock_flash_driver_io_expect_write_read(
  const uint8_t *const output_data,
  const uint8_t *const input_data,
  const uint16_t output_data_size,
  const uint16_t input_data_size
);
void mock_flash_driver_io_expect_select(bool select);
void mock_flash_driver_io_verify_complete(void);

#endif
