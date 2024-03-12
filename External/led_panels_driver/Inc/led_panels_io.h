#ifndef LED_PANELS_IO_H
#define LED_PANELS_IO_H

#include <stdint.h>
#include "led_panels_defs.h"

// led_panels_status hc06_io_read(
//   uint8_t *const data,
//   const uint16_t data_size
// );
// led_panels_status hc06_io_write(
//   const uint8_t *const data,
//   const uint16_t data_size
// );
// led_panels_status hc06_io_set_baudrate(const uint32_t baudrate);

led_panels_status led_panels_io_send_data(
  const uint8_t *const data,
  const uint16_t data_size
);

#endif
