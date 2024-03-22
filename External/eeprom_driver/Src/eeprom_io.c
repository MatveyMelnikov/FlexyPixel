#include "eeprom_io.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal_i2c.h"

extern I2C_HandleTypeDef *eeprom_i2c;

eeprom_status eeprom_io_read(
    uint8_t *const data,
    const uint8_t data_size
)
{
    return (eeprom_status)HAL_I2C_Master_Receive(
        eeprom_i2c, DEV_ADDR, data, data_size, EEPROM_TIMEOUT
    );
}

eeprom_status eeprom_io_write(
    const uint8_t *const data,
    const uint8_t data_size
)
{
    return (eeprom_status)HAL_I2C_Master_Transmit(
        eeprom_i2c, DEV_ADDR, (uint8_t*)data, data_size, EEPROM_TIMEOUT
    );
}
