#ifndef RENDER_CONTROLLER_H
#define RENDER_CONTROLLER_H

#include "stm32f1xx_hal.h"
#include "mode_handler.h"
#include <stdint.h>

void render_controller_create(
  mode_handler *const handlers,
  uint8_t handlers_num
);
HAL_StatusTypeDef render_controller_process(void);
void render_controller_send_frame_complete(void);
void render_controller_receive_complete(void);

#endif
