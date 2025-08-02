#ifndef FRAMES_REPO_DEFS_H
#define FRAMES_REPO_DEFS_H

#include <stdint.h>
#include "led_panels_defs.h"
#include "displays_config_repo_defs.h"

enum {
  FRAMES_REPO_START_BYTE = 0x99U,
  FRAMES_REPO_MAX_AMOUNT = 0x25cU,
  FRAMES_REPO_FRAME_DATA_SIZE = 96U *
    DISPLAYS_CONFIG_REPO_DISPLAYS_MAX_AMOUNT,
  FRAMES_REPO_FRAME_SIZE = 1U + FRAMES_REPO_FRAME_DATA_SIZE,
};

typedef enum
{
  FRAMES_REPO_OK = 0U,
  FRAMES_REPO_ERROR,
  FRAMES_REPO_INVALID
} frames_repo_status;

typedef struct
{
  uint8_t start_byte;
  uint8_t *data;
} frames_repo_data;

typedef struct
{
  // data
  frames_repo_status (*save)(
    const uint32_t frame_index,
    const frames_repo_data *const
  );
  // data
  frames_repo_status (*load)(
    const uint32_t frame_index,
    frames_repo_data *const
  );
} frames_repo_io;

#endif
