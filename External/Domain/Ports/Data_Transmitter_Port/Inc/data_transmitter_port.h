#ifndef DATA_TRANSMITTER_H
#define DATA_TRANSMITTER_H

#include "data_transmitter_port_defs.h"

void data_transmitter_port_create(data_transmitter_port_io io);
void data_transmitter_port_destroy(void);
data_transmitter_port_status data_transmitter_port_write(
  const uint8_t *const data,
  const uint16_t size
);
data_transmitter_port_status data_transmitter_port_read(
  uint8_t *const data,
  const uint16_t size
);
bool data_transmitter_port_is_data_received(void);

#endif
