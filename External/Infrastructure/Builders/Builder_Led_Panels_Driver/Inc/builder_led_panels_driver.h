#ifndef BUILDER_LED_PANELS_DRIVER_H
#define BUILDER_LED_PANELS_DRIVER_H

#include "stm32f1xx_hal.h"

void builder_led_panels_driver_build(TIM_HandleTypeDef *led_panels_tim);
void builder_led_panels_driver_destroy(void);

#endif
