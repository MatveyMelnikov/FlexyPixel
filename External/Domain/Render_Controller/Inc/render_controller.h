#ifndef RENDER_CONTROLLER_H
#define RENDER_CONTROLLER_H

#include "render_controller_defs.h"
#include <stdbool.h>

void render_controller_create(void);
void render_controller_update_config(void);
void render_controller_destroy(void);
void render_controller_reset(void);
render_controller_status render_controller_render(void);
void render_controller_pause(bool is_paused);

#endif
