#ifndef MOCK_RENDER_CONTROLLER_IO_H
#define MOCK_RENDER_CONTROLLER_IO_H

#include "render_controller_io.h"

void mock_render_controller_io_create(const uint8_t max_expectations);
void mock_render_controller_io_destroy(void);
void mock_render_controller_io_expect_start_timer(void);
void mock_render_controller_io_expect_stop_timer(void);
void mock_render_controller_io_set_timeout(void);
void mock_render_controller_io_verify_complete(void);

#endif