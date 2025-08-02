#include "builder_general.h"
#include "builder_hc06_driver.h"
#include "builder_cy15b104q_driver.h"
#include "builder_led_panels_driver.h"
#include "builder_data_transmitter_port.h"
#include "builder_debug_handler.h"
#include <stdbool.h>

// Static variables ----------------------------------------------------------

// Static prototypes ---------------------------------------------------------

// Implementations -----------------------------------------------------------

void builder_general_build(
  UART_HandleTypeDef *const hc06_uart,
  SPI_HandleTypeDef *const mem_spi,
  TIM_HandleTypeDef *led_panels_tim
)
{
  builder_hc06_driver_build(hc06_uart);
  builder_cy15b104q_driver_build(mem_spi);
  builder_led_panels_driver_build(led_panels_tim);
  builder_data_transmitter_port_build();
  builder_debug_handler_build();
}

void builder_general_destroy()
{
  builder_data_transmitter_port_destroy();
  builder_led_panels_driver_destroy();
  builder_cy15b104q_driver_destroy();
  builder_hc06_driver_destroy();
}
