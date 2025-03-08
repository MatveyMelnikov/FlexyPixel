#include "main.h"
#include "builder_cy15b104q_driver.h"
#include "cy15b104q_driver.h"

// Static variables ----------------------------------------------------------

static SPI_HandleTypeDef *module_spi;

// Static prototypes ---------------------------------------------------------

cy15b104q_driver_status cy15b104q_transmit(
  const uint8_t *const data,
  const uint16_t size,
  const uint32_t timeout
);
cy15b104q_driver_status cy15b104q_receive(
  uint8_t *const data,
  const uint16_t size,
  const uint32_t timeout
);
void cy15b104q_write_cs_pin(const bool is_set);

// Implementations -----------------------------------------------------------

void builder_cy15b104q_driver_build(SPI_HandleTypeDef *const mem_spi)
{
  module_spi = mem_spi;

  cy15b104q_driver_init_module(
    (cy15b104q_driver_io_struct) {
      .transmit = cy15b104q_transmit,
      .receive = cy15b104q_receive,
      .write_cs_pin = cy15b104q_write_cs_pin,
      .delay = HAL_Delay
    }
  );
}

void builder_cy15b104q_driver_destroy()
{
  cy15b104q_driver_deinit_module();
}

cy15b104q_driver_status cy15b104q_transmit(
  const uint8_t *const data,
  const uint16_t size,
  const uint32_t timeout
)
{
  return (cy15b104q_driver_status)HAL_SPI_Transmit(
    module_spi,
    (uint8_t*)data,
    size,
    timeout
  );
}

cy15b104q_driver_status cy15b104q_receive(
  uint8_t *const data,
  const uint16_t size,
  const uint32_t timeout
)
{
  return (cy15b104q_driver_status)HAL_SPI_Receive(
    module_spi,
    data,
    size,
    timeout
  );
}

void cy15b104q_write_cs_pin(const bool is_set)
{
  HAL_GPIO_WritePin(
    CY15B104Q_NCS_GPIO_Port,
    CY15B104Q_NCS_Pin,
    is_set ? GPIO_PIN_SET : GPIO_PIN_RESET
  );
}
