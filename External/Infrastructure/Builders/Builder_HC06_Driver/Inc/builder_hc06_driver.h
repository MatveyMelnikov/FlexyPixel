#ifndef BUILDER_HC06_DRIVER_H
#define BUILDER_HC06_DRIVER_H

#include "stm32f1xx_hal.h"

void builder_hc06_driver_build(UART_HandleTypeDef *const hc06_uart);
void builder_hc06_driver_destroy(void);

#endif
