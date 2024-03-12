#ifndef HC06_IO_H
#define HC06_IO_H

#include <stdint.h>
#include "hc06_defs.h"

hc06_status hc06_io_read(
  uint8_t *const data,
  const uint16_t data_size
);
hc06_status hc06_io_write(
  const uint8_t *const data,
  const uint16_t data_size
);
hc06_status hc06_io_set_baudrate(const uint32_t baudrate);

#endif
