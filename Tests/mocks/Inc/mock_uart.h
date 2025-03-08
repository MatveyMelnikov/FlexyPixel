#ifndef MOCK_UART_H
#define MOCK_UART_H

#include <stdint.h>

typedef enum {
  MOCK_UART_OK = 0x00U,
  MOCK_UART_ERROR = 0x01U,
  MOCK_UART_BUSY = 0x02U,
  MOCK_UART_TIMEOUT = 0x03U
} mock_uart_status;

void mock_uart_create(const uint8_t max_expectations);
void mock_uart_destroy(void);
void mock_uart_expect_transmit(
  const uint8_t *const data,
  const uint16_t size
);
void mock_uart_expect_receive(
  const uint8_t * const data,
  const uint16_t size
);
void mock_uart_expect_set_baudrate(uint32_t *const baudrate);
void mock_uart_verify_complete(void);

mock_uart_status mock_uart_transmit(uint8_t *const data, const uint16_t size);
mock_uart_status mock_uart_receive(uint8_t *const data, const uint16_t size);
mock_uart_status mock_uart_set_baudrate(uint32_t baudrate);

#endif
