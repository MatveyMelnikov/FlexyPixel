#include "data_transmitter_port.h"
#include "debug_handler.h"

// Defines -------------------------------------------------------------------

// Static variables ----------------------------------------------------------

static data_transmitter_port_io module_io;

// Static functions ----------------------------------------------------------

// Implementations -----------------------------------------------------------

void data_transmitter_port_create(data_transmitter_port_io io)
{
  module_io = io;
}

void data_transmitter_port_destroy(void)
{
  module_io = (data_transmitter_port_io) { 0 };
}

data_transmitter_port_status data_transmitter_port_write(
  const uint8_t *const data,
  const uint16_t size
)
{
  DEBUG_HANDLER_FORMAT_OUTPUT("\r\n\toutput: %s", (char*)data);
  return module_io.write(data, size);
}

data_transmitter_port_status data_transmitter_port_read(
  uint8_t *const data,
  const uint16_t size
)
{
  return module_io.read(data, size);
}

bool data_transmitter_port_is_data_received()
{
  return module_io.is_data_received();
}
