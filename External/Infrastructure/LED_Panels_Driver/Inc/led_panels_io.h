#ifndef LED_PANELS_IO_H
#define LED_PANELS_IO_H

#include <stdint.h>
#include "led_panels_defs.h"

led_panels_status led_panels_io_send_data(
  const uint8_t *const data,
  const uint16_t data_size
);

void led_panels_io_stop_sending_data(void);

#endif
