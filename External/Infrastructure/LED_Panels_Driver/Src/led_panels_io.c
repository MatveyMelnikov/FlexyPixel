#include "led_panels_io.h"
#include "stm32f1xx.h"

extern TIM_HandleTypeDef *led_panels_tim;

led_panels_status led_panels_io_send_data(
  const uint8_t *const data,
  const uint16_t data_size
)
{
  return (led_panels_status)HAL_TIM_PWM_Start_DMA(
    led_panels_tim,
    TIM_CHANNEL_1,
    (uint32_t *)data,
    data_size
  );
}

void led_panels_io_stop_sending_data()
{
	HAL_TIM_PWM_Stop_DMA(led_panels_tim, TIM_CHANNEL_1);
  led_panels_tim->Instance->CCR1 = 0; // period
}
