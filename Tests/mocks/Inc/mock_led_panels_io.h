#ifndef MOCK_LED_PANELS_IO_H
#define MOCK_LED_PANELS_IO_H

#include "led_panels_io.h"

void mock_led_panels_io_create(const uint8_t max_expectations);
void mock_led_panels_io_destroy(void);
void mock_led_panels_io_expect_send_data(
  const uint8_t *const data,
  const uint16_t data_size
);
void mock_led_panels_io_verify_complete(void);

#endif
