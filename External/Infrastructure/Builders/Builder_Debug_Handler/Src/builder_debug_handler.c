#include "main.h"
#include "stm32f1xx_hal.h"
#include "usbd_cdc_if.h"
#include "builder_debug_handler.h"
#include "debug_handler.h"

// Defines -------------------------------------------------------------------

enum {
  USB_RESET_DELAY = 20U,
};

// Static variables ----------------------------------------------------------

// Static prototypes ---------------------------------------------------------

static debug_handler_status debug_output(uint8_t *data, uint16_t len);
static void debug_reset(bool is_reset);
static void builder_debug_handler_wait_before_connection(void);

// Implementations -----------------------------------------------------------

void builder_debug_handler_build()
{
  debug_output_create(
    (debug_handler_io) {
      .output = debug_output,
      .reset = debug_reset,
      .wait_before_connection = builder_debug_handler_wait_before_connection,
      .delay = HAL_Delay
    }
  );
}

void builder_debug_handler_destroy()
{
  debug_output_destroy();
}

static debug_handler_status debug_output(uint8_t *data, uint16_t len)
{
  return (debug_handler_status)CDC_Transmit_FS(data, len);
}

static void debug_reset(bool is_reset)
{
  HAL_GPIO_WritePin(
    USB_NRST_GPIO_Port,
    USB_NRST_Pin,
    is_reset ? GPIO_PIN_RESET : GPIO_PIN_SET
  );
  HAL_Delay(USB_RESET_DELAY);
}

static void builder_debug_handler_wait_before_connection()
{
  const char *test_str = "-\r\n";

  for (uint8_t i = 0; i < 100; i++)
  {
    if (CDC_Transmit_FS((uint8_t*)test_str, strlen(test_str)) == 0)
      break;
    HAL_Delay(10);
  }
}
