#include "builder_hc06_driver.h"
#include "hc06_driver.h"

// Static variables ----------------------------------------------------------

static UART_HandleTypeDef *module_uart;

// Static prototypes ---------------------------------------------------------

static hc06_status hc06_io_blocking_read(
  uint8_t *const data,
  const uint16_t data_size
);
static hc06_status hc06_io_write(
  const uint8_t *const data,
  const uint16_t data_size
);
static hc06_status hc06_io_non_blocking_read(
  uint8_t *const data,
  const uint16_t data_size
);
hc06_status hc06_io_set_baudrate(const uint32_t baudrate);

// Implementations -----------------------------------------------------------

void builder_hc06_driver_build(UART_HandleTypeDef *const hc06_uart)
{
  module_uart = hc06_uart;

  hc06_create(
    (hc06_io) {
      .write = hc06_io_write,
      .non_blocking_read = hc06_io_non_blocking_read,
      .blocking_read = hc06_io_blocking_read,
      .set_controller_baudrate = hc06_io_set_baudrate,
      .delay = HAL_Delay
    }
  );
}

static hc06_status hc06_io_blocking_read(
  uint8_t *const data,
  const uint16_t data_size
)
{
  return (hc06_status)HAL_UART_Receive(
    module_uart, data, data_size, HC06_TIMEOUT
  );
}

static hc06_status hc06_io_write(
  const uint8_t *const data,
  const uint16_t data_size
)
{
  return (hc06_status)HAL_UART_Transmit(
    module_uart, data, data_size, HC06_TIMEOUT
  );
}

static hc06_status hc06_io_non_blocking_read(
  uint8_t *const data,
  const uint16_t data_size
)
{
  return (hc06_status)HAL_UART_Receive_DMA(module_uart, data, data_size);
}

hc06_status hc06_io_set_baudrate(const uint32_t baudrate)
{
  HAL_UART_Abort_IT(module_uart);
  HAL_UART_DeInit(module_uart);
  module_uart->Init.BaudRate = baudrate;

  hc06_status status = (hc06_status)HAL_UART_Init(module_uart);
  return status;
}

void builder_hc06_driver_destroy()
{
  module_uart = NULL;

  hc06_destroy();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  hc06_receive_complete();
}