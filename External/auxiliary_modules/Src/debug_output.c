#include "debug_output.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal_uart.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>

extern UART_HandleTypeDef *debug_uart;

// Static variables ----------------------------------------------------------

static char *output_format = "-(%s, %d): %s\r\n";
static char output_buffer[OUTPUT_BUFFER_SIZE];
static char internal_buffer[INTERNAL_BUFFER_SIZE];

// Implementations -----------------------------------------------------------

void debug_output_send(
  char *msg,
  uint16_t len,
  uint16_t line,
  char *file
)
{
  if (len > INTERNAL_BUFFER_SIZE)
    len = INTERNAL_BUFFER_SIZE - 1;

  memcpy(internal_buffer, msg, len);
  *(internal_buffer + len) = '\0';
  sprintf(output_buffer, output_format, file, line, internal_buffer);

  HAL_UART_Transmit(
    debug_uart,
    (uint8_t*)output_buffer,
    strlen(output_buffer),
    DEBUG_TIMEOUT
  );
}

void debug_output_with_insert_send(
  char *format,
  char *insertion,
  uint16_t line,
  char *file
)
{
  sprintf(internal_buffer, format, insertion);
  sprintf(output_buffer, output_format, file, line, internal_buffer);

  HAL_UART_Transmit(
    debug_uart,
    (uint8_t*)output_buffer,
    strlen(output_buffer),
    DEBUG_TIMEOUT
  );
}
