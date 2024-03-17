#include "render_controller_io.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal_tim.h"
#include "hc06_driver.h"

extern TIM_HandleTypeDef *render_controller_tim;
static led_panels_buffer **front_buffer = NULL;
static volatile bool is_transmit_timeout = false;

void render_controller_io_create(led_panels_buffer **buffer)
{
  front_buffer = buffer; // ?
}

void render_controller_io_start_timeout_timer()
{
  if (render_controller_tim == NULL)
    return;
  
  is_transmit_timeout = false;
  render_controller_io_stop_timeout_timer();

  // __HAL_TIM_CLEAR_FLAG(render_controller_tim, TIM_SR_UIF);
  // HAL_TIM_Base_Start_IT(render_controller_tim);
}

void render_controller_io_stop_timeout_timer()
{
  if (render_controller_tim == NULL)
    return;
  
  HAL_TIM_Base_Stop_IT(render_controller_tim);
  is_transmit_timeout = false;
}

void render_controller_io_timeout_timer_complete()
{
  is_transmit_timeout = true;
}

bool render_controller_io_is_timeout()
{
  return is_transmit_timeout;
}

void render_controller_io_send_frame_complete()
{
  if (*front_buffer)
    led_panels_send_complete(*front_buffer);
}

void render_controller_io_receive_complete()
{
  hc06_receive_complete();
}

uint32_t render_controller_io_get_ticks()
{
  return HAL_GetTick();
}

void render_controller_io_destroy()
{
  front_buffer = NULL;
}
