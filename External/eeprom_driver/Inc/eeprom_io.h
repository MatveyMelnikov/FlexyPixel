#ifndef EEPROM_IO_H
#define EEPROM_IO_H

#include <stdint.h>
#include "eeprom_defs.h"

// The microcontroller can transmit up to 63 more data words 
// (ATMEL AT24C8C128 datasheet pg. 7)
eeprom_status eeprom_io_read(
    uint8_t *const data,
    const uint8_t data_size
);
eeprom_status eeprom_io_write(
    const uint8_t *const data,
    const uint8_t data_size
);

#endif
