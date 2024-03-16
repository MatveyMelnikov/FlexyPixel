#ifndef MOCK_HC06_IO_H
#define MOCK_HC06_IO_H

#include "hc06_io.h"

void mock_hc06_io_create(const uint8_t max_expectations);
void mock_hc06_io_destroy(void);
void mock_hc06_io_expect_write(
  const uint8_t *const data,
  const uint16_t data_size
);
void mock_hc06_io_expect_read_then_return(
  const uint8_t *const data,
  const uint16_t data_size
);
void mock_hc06_io_expect_read_external_then_return(
  const uint8_t *const data,
  const uint16_t data_size
);
void mock_hc06_io_expect_baudrate_change(void);
void mock_hc06_io_verify_complete(void);

#endif
