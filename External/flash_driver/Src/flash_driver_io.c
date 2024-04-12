#include "flash_driver_io.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal_spi.h"
#include "stm32f1xx_hal_gpio.h"

extern SPI_HandleTypeDef *flash_driver_spi;

// Implementations -----------------------------------------------------------

flash_driver_status flash_driver_io_write(
  uint8_t *const output_data,
  const uint16_t output_data_size
)
{
  return (flash_driver_status)HAL_SPI_Transmit(
    flash_driver_spi,
    output_data,
    output_data_size,
    FLASH_DRIVER_TIMEOUT
  );
}

flash_driver_status flash_driver_io_write_read(
  uint8_t *const output_data,
  uint8_t *const input_data,
  const uint16_t output_data_size
)
{
  return (flash_driver_status)HAL_SPI_TransmitReceive(
    flash_driver_spi,
    output_data,
    input_data,
    output_data_size,
    FLASH_DRIVER_TIMEOUT
  );
}

void flash_driver_select(bool select)
{
  HAL_GPIO_WritePin(
    GPIOB,
    GPIO_PIN_0,
    select ? GPIO_PIN_RESET : GPIO_PIN_SET
  );
}
