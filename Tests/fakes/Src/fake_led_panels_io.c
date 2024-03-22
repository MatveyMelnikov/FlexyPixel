#include "fake_led_panels_io.h"

// Implementations from led_panels_io ----------------------------------------

led_panels_status led_panels_io_send_data(
  const uint8_t *const data,
  const uint16_t data_size
)
{
  return LED_PANELS_OK;
}

void led_panels_io_stop_sending_data()
{
}
