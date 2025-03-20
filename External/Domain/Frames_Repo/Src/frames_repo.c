#include "frames_repo.h"
#include <string.h>

// Static variables ----------------------------------------------------------

static frames_repo_io module_io;
static frames_repo_data internal_data;
static uint8_t pixel_data[FRAMES_REPO_FRAME_DATA_SIZE];

// Static prototypes ---------------------------------------------------------

static bool frames_repo_is_out_of_bounds(uint32_t frame_index);
static bool frames_repo_is_start_byte_invalid(void);

// Implementations -----------------------------------------------------------

void frames_repo_create(frames_repo_io io)
{
  module_io = io;
  internal_data = (frames_repo_data) { 0 };
  internal_data.start_byte = FRAMES_REPO_START_BYTE;
  internal_data.data = pixel_data;
}

void frames_repo_destroy()
{
  module_io = (frames_repo_io) { 0 };
  internal_data = (frames_repo_data) { 0 };
  internal_data.data = NULL;
}

frames_repo_status frames_repo_get(
  uint32_t frame_index,
  uint8_t *const data,
  uint16_t data_size
)
{
  if (frames_repo_is_out_of_bounds(frame_index))
    return FRAMES_REPO_ERROR;

  frames_repo_status status = module_io.load(frame_index, &internal_data);

  if (frames_repo_is_start_byte_invalid())
    return FRAMES_REPO_ERROR;

  memcpy(data, pixel_data, data_size);

  return status;
}

frames_repo_status frames_repo_save(
  uint32_t frame_index,
  const uint8_t *const data,
  uint16_t data_size
)
{
  if (frames_repo_is_out_of_bounds(frame_index))
    return FRAMES_REPO_ERROR;

  memcpy(pixel_data, data, data_size);
  internal_data.start_byte = FRAMES_REPO_START_BYTE;
  return module_io.save(frame_index, &internal_data);
}

static bool frames_repo_is_out_of_bounds(uint32_t frame_index)
{
  if (frame_index > FRAMES_REPO_MAX_AMOUNT)
    return true;
  
  return false;
}

static bool frames_repo_is_start_byte_invalid()
{
  if (internal_data.start_byte == FRAMES_REPO_START_BYTE)
    return false;

  return true;
}

void frames_repo_reset(uint32_t frame_index)
{
  memset(pixel_data, 0U, sizeof(pixel_data));
  (void)module_io.save(frame_index, &internal_data);
}
