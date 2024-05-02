#ifndef RENDER_CONTROLLER_H
#define RENDER_CONTROLLER_H

#include "task.h"
#include "render_controller_io.h"
#include <stdint.h>

void render_controller_create(
  task *const tasks_list,
  uint8_t tasks_amount
);
void render_controller_destroy(void);
bool render_controller_process(void);

#endif
