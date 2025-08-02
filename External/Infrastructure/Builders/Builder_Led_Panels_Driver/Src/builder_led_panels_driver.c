#include "builder_led_panels_driver.h"
#include "led_panels_driver.h"

// Static variables ----------------------------------------------------------

static TIM_HandleTypeDef *module_tim;

// Static prototypes ---------------------------------------------------------

static led_panels_status led_panels_io_send_data(
  const uint8_t *const data,
  const uint16_t data_size
);

static void led_panels_io_stop_sending_data(void);

// Implementations -----------------------------------------------------------

void builder_led_panels_driver_build(TIM_HandleTypeDef *led_panels_tim)
{
  module_tim = led_panels_tim;

  led_panels_init(
    (led_panels_io) {
      .send = led_panels_io_send_data,
      .stop_sending = led_panels_io_stop_sending_data
    }
  );
}

void builder_led_panels_driver_destroy()
{
  led_panels_deinit();
}

static led_panels_status led_panels_io_send_data(
  const uint8_t *const data,
  const uint16_t data_size
)
{
  return (led_panels_status)HAL_TIM_PWM_Start_DMA(
    module_tim,
    TIM_CHANNEL_1,
    (uint32_t *)data,
    data_size
  );
}

static void led_panels_io_stop_sending_data()
{
	HAL_TIM_PWM_Stop_DMA(module_tim, TIM_CHANNEL_1);
  module_tim->Instance->CCR1 = 0; // period
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  led_panels_send_complete();
}

void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
  led_panels_half_send_complete();
}
