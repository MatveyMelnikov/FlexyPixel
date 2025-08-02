#ifndef FRAMES_REPO_H
#define FRAMES_REPO_H

#include "frames_repo_defs.h"

void frames_repo_create(frames_repo_io io);
void frames_repo_destroy(void);
frames_repo_status frames_repo_get(
  uint32_t frame_index,
  uint8_t *const data,
  uint16_t data_size
);
frames_repo_status frames_repo_save(
  uint32_t frame_index,
  const uint8_t *const data,
  uint16_t data_size
);
void frames_repo_reset(uint32_t frame_index);

#endif
