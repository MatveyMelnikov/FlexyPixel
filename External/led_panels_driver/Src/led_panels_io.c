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
