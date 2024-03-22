#ifndef EEPROM_DRIVER_H
#define EEPROM_DRIVER_H

#include <stdint.h>
#include "eeprom_defs.h"

eeprom_status eeprom_addr_write(
    uint16_t addr
);
eeprom_status eeprom_byte_write(
    uint16_t addr,
    uint8_t data
);
eeprom_status eeprom_page_write(
    const uint16_t addr,
    const uint8_t *const data,
    uint16_t size
);
eeprom_status eeprom_current_address_read(
    uint8_t *const data
);
eeprom_status eeprom_random_byte_read(
    const uint16_t addr,
    uint8_t *const data
);
eeprom_status eeprom_sequential_read(
    uint8_t *const data,
    const uint16_t size
);
eeprom_status eeprom_check_link(void);

#endif
