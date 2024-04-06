#ifndef EEPROM_DEFS_H
#define EEPROM_DEFS_H

enum
{
    EEPROM_TIMEOUT = 0x64U,
    EEPROM_SIZE = 0x8000U,
    EEPROM_PAGE_SIZE = 64U
};

#define DEV_ADDR 0xA0U
#define VOLTAGE_1_8_V

#ifdef VOLTAGE_1_8_V
#define WRITE_CYCLE_TIMEOUT 0x14U
#else
#define WRITE_CYCLE_TIMEOUT 0xaU
#endif

typedef enum
{
    EEPROM_OK = 0x00U,
    EEPROM_ERROR = 0x01U,
    EEPROM_BUSY = 0x02U,
    EEPROM_TIMEOUT_ERROR = 0x03U,
    EEPROM_OVERFLOW = 0x04U
} eeprom_status;

#endif
