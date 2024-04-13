#ifndef RENDER_CONTROLLER_H
#define RENDER_CONTROLLER_H

#include "handler.h"
#include "render_controller_io.h"
#include <stdint.h>

void render_controller_set_delay(const uint32_t delay);
void render_controller_create(
  handler *const handlers,
  uint8_t handlers_num
);
void render_controller_destroy(void);
bool render_controller_process(void);

#endif
