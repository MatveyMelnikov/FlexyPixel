#include "eeprom_time.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal_i2c.h"

void eeprom_delay(uint32_t delay)
{
    HAL_Delay(delay);
}
