#ifndef DATA_TRANSMITTER_PORT_DEFS_H
#define DATA_TRANSMITTER_PORT_DEFS_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
  DATA_TRANSMITTER_PORT_STATUS_OK = 0x00U,
  DATA_TRANSMITTER_PORT_STATUS_ERROR = 0x01U,
  DATA_TRANSMITTER_PORT_STATUS_BUSY = 0x02U,
  DATA_TRANSMITTER_PORT_STATUS_TIMEOUT_ERROR = 0x03U,
  DATA_TRANSMITTER_PORT_STATUS_OVERFLOW = 0x04U
} data_transmitter_port_status;

typedef struct
{
  data_transmitter_port_status (*write)(const uint8_t *const, const uint16_t);
  data_transmitter_port_status (*read)(uint8_t *const, const uint16_t);
  bool (*is_data_received)(void);
} data_transmitter_port_io;

#endif
