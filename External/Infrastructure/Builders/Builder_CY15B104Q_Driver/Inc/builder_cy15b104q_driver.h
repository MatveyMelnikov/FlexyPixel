#ifndef BUILDER_CY15B104Q_DRIVER_H
#define BUILDER_CY15B104Q_DRIVER_H

#include "stm32f1xx_hal.h"

void builder_cy15b104q_driver_build(SPI_HandleTypeDef *const mem_spi);
void builder_cy15b104q_driver_destroy(void);

#endif
