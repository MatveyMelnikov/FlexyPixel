#include "builder_data_transmitter_port.h"
#include "data_transmitter_port.h"
#include "hc06_driver.h"

// Static variables ----------------------------------------------------------

// Static prototypes ---------------------------------------------------------

static data_transmitter_port_status transmitter_write(
  const uint8_t *const data,
  const uint16_t size
);
static data_transmitter_port_status transmitter_read(
  uint8_t *const data,
  const uint16_t size
);
static bool transmitter_is_data_received(void);

// Implementations -----------------------------------------------------------

void builder_data_transmitter_port_build()
{
  data_transmitter_port_create(
    (data_transmitter_port_io) {
      .write = transmitter_write,
      .read = transmitter_read,
      .is_data_received = transmitter_is_data_received
    }
  );
}

void builder_data_transmitter_port_destroy()
{
  data_transmitter_port_destroy();
}

static data_transmitter_port_status transmitter_write(
  const uint8_t *const data,
  const uint16_t size
)
{
  return (data_transmitter_port_status)hc06_write(data, size);
}

static data_transmitter_port_status transmitter_read(
  uint8_t *const data,
  const uint16_t size
)
{
  return (data_transmitter_port_status)hc06_read(data, size);
}

static bool transmitter_is_data_received()
{
  return hc06_is_data_received();
}
