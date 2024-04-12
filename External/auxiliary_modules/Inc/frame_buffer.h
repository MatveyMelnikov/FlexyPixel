#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "led_panels_driver.h"

enum {
  FRAME_BUFFER_SIZE = 896U, // 14 pages - start byte + data (8x8 panel)
  MEMORY_PAGE_SIZE = 256U,
  MEMORY_SECTOR_SIZE = 4096U
};

typedef enum
{
  FRAME_BUFFER_OK = 0x0U,
  FRAME_BUFFER_IN_PROGRESS = 0x01U,
  FRAME_BUFFER_ERROR = 0x02U,
  FRAME_BUFFER_LOCK = 0x03U
} frame_buffer_status;

void frame_buffer_set_frames_num(uint8_t num);
void frame_buffer_set(const uint8_t *data);
void frame_buffer_lock(bool is_locked);
bool frame_buffer_is_locked(void);
bool frame_buffer_is_busy(void);
frame_buffer_status frame_buffer_save(void);
frame_buffer_status frame_buffer_load(led_panels_buffer *const buffer);
void frame_buffer_load_conf(void);
void frame_buffer_reset(void);

#endif
