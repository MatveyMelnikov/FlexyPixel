#include "mock_uart.h"
#include "mock.h"
#include "unity_fixture.h"
#include <string.h>
#include <stdio.h>

// Defines -------------------------------------------------------------------

enum {
  OUTPUT_CHARS_ADDITIVE = 50U
};

// Static variables ----------------------------------------------------------

static const char *name = "UART";
static mock uart_mock;

// Static prototypes ---------------------------------------------------------

// Implementations -----------------------------------------------------------

void mock_uart_create(const uint8_t max_expectations)
{
  uart_mock = mock_create(name, max_expectations);
}

void mock_uart_destroy()
{
  mock_destroy(&uart_mock);
}

void mock_uart_expect_transmit(
  const uint8_t *const data,
  const uint16_t size
)
{
  mock_record_expectation(&uart_mock, WRITE, data, (uint8_t)size);
}

void mock_uart_expect_receive(
  const uint8_t * const data,
  const uint16_t size
)
{
  mock_record_expectation(&uart_mock, READ, data, (uint8_t)size);
}

void mock_uart_expect_set_baudrate(uint32_t *const baudrate)
{
  mock_record_expectation(
    &uart_mock,
    SET,
    (uint8_t*)baudrate,
    sizeof(uint32_t)
  );
}

void mock_uart_verify_complete()
{
  mock_verify_complete(&uart_mock);
}

mock_uart_status mock_uart_transmit(uint8_t *const data, const uint16_t size)
{
  expectation current_expectation = mock_get_expectation(&uart_mock);

  mock_check_kind(&uart_mock, &current_expectation, WRITE);
  mock_check_data(&uart_mock, &current_expectation, data, size);

  return MOCK_UART_OK;
}

mock_uart_status mock_uart_receive(uint8_t *const data, const uint16_t size)
{
  expectation current_expectation = mock_get_expectation(&uart_mock);

  mock_check_kind(&uart_mock, &current_expectation, READ);
  mock_check_data_size(&uart_mock, &current_expectation, size);

  memcpy(data, current_expectation.data, size);

  return MOCK_UART_OK;
}

mock_uart_status mock_uart_set_baudrate(uint32_t baudrate)
{
  expectation current_expectation = mock_get_expectation(&uart_mock);

  mock_check_kind(&uart_mock, &current_expectation, SET);
  mock_check_data(
    &uart_mock,
    &current_expectation,
    (uint8_t*)&baudrate,
    sizeof(uint32_t)
  );

  return MOCK_UART_OK;
}
