#ifndef MOCK_EEPROM_IO_H
#define MOCK_EEPROM_IO_H

#include "eeprom_io.h"

void mock_eeprom_io_create(const uint8_t max_expectations);
void mock_eeprom_io_destroy(void);
void mock_eeprom_io_expect_write(
    const uint8_t *const data,
    const uint8_t data_size
);
void mock_eeprom_io_expect_read_then_return(
    const uint8_t *const data,
    const uint8_t data_size
);
void mock_eeprom_io_verify_complete(void);

#endif
