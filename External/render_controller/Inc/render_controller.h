#ifndef RENDER_CONTROLLER_H
#define RENDER_CONTROLLER_H

#include "mode_handler.h"
#include "render_controller_io.h"
#include <stdint.h>

void render_controller_create(
  mode_handler *const handlers,
  uint8_t handlers_num
);
bool render_controller_process(void);

#endif
