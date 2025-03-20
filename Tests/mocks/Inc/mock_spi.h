#ifndef MOCK_SPI_H
#define MOCK_SPI_H

#include <stdint.h>

typedef enum {
  MOCK_SPI_OK = 0x00U,
  MOCK_SPI_ERROR = 0x01U,
  MOCK_SPI_BUSY = 0x02U,
  MOCK_SPI_TIMEOUT = 0x03U
} mock_spi_status;

void mock_spi_create(const uint8_t max_expectations);
void mock_spi_destroy(void);
void mock_spi_expect_transmit(
  const uint8_t *const data,
  const uint16_t size
);
void mock_spi_expect_receive(
  const uint8_t * const data,
  const uint16_t size
);
void mock_spi_verify_complete(void);

mock_spi_status mock_spi_transmit(uint8_t *const data, const uint16_t size);
mock_spi_status mock_spi_receive(uint8_t *const data, const uint16_t size);

#endif
