#include "mock_spi.h"
#include "mock.h"
#include "unity_fixture.h"
#include <string.h>
#include <stdio.h>

// Defines -------------------------------------------------------------------

enum {
  OUTPUT_CHARS_ADDITIVE = 50U
};

// Static variables ----------------------------------------------------------

static const char *name = "SPI";
static mock spi_mock;

// Static prototypes ---------------------------------------------------------

// Implementations -----------------------------------------------------------

void mock_spi_create(const uint8_t max_expectations)
{
  spi_mock = mock_create(name, max_expectations);
}

void mock_spi_destroy()
{
  mock_destroy(&spi_mock);
}

void mock_spi_expect_transmit(
  const uint8_t *const data,
  const uint16_t size
)
{
  mock_record_expectation(&spi_mock, SET, data, (uint8_t)size);
}

void mock_spi_expect_receive(
  const uint8_t * const data,
  const uint16_t size
)
{
  mock_record_expectation(&spi_mock, GET, data, (uint8_t)size);
}

void mock_spi_verify_complete()
{
  mock_verify_complete(&spi_mock);
}

mock_spi_status mock_spi_transmit(uint8_t *const data, const uint16_t size)
{
  expectation current_expectation = mock_get_expectation(&spi_mock);

  mock_check_kind(&spi_mock, &current_expectation, SET);
  mock_check_data(&spi_mock, &current_expectation, data, size);

  return MOCK_SPI_OK;
}

mock_spi_status mock_spi_receive(uint8_t *const data, const uint16_t size)
{
  expectation current_expectation = mock_get_expectation(&spi_mock);

  mock_check_kind(&spi_mock, &current_expectation, GET);
  mock_check_data_size(&spi_mock, &current_expectation, size);

  memcpy(data, current_expectation.data, size);

  return MOCK_SPI_OK;
}
