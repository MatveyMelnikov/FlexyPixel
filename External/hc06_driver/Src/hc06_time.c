#include "hc06_time.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal_i2c.h"

void hc06_delay(uint32_t delay)
{
  HAL_Delay(delay);
}
