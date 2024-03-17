#ifndef RENDER_CONTROLLER_IO_H
#define RENDER_CONTROLLER_IO_H

#include "led_panels_driver.h"

void render_controller_io_create(led_panels_buffer **buffer);
void render_controller_io_start_timeout_timer(void);
void render_controller_io_stop_timeout_timer(void);
void render_controller_io_timeout_timer_complete(void);
bool render_controller_io_is_timeout(void);
void render_controller_io_send_frame_complete(void);
void render_controller_io_receive_complete(void);
uint32_t render_controller_io_get_ticks(void);
void render_controller_io_destroy(void);

#endif
