#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "led_panels_driver.h"

enum {
  FRAME_BUFFER_SIZE = 896U // 14 pages - start byte + data (8x8 panel)
};

void frame_buffer_set_frames_num(uint8_t num);
void frame_buffer_set(const uint8_t *data);
bool frame_buffer_is_saved(void);
// As a result, true if the data is sent for storage.
// Otherwise - an error or all frames have already been saved
bool frame_buffer_save(void);
bool frame_buffer_load(led_panels_buffer *const buffer);
void frame_buffer_load_conf(void);
void frame_buffer_reset(void);

#endif
