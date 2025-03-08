#ifndef BUILDER_GENERAL_H
#define BUILDER_GENERAL_H

#include "stm32f1xx_hal.h"

void builder_general_build(
  UART_HandleTypeDef *const hc06_uart,
  SPI_HandleTypeDef *const mem_spi,
  TIM_HandleTypeDef *led_panels_tim
);
void builder_general_destroy(void);

#endif
