#include "frames_storage.h"
#include "cy15b104q_driver.h"
#include "frames_repo.h"

// Static variables ----------------------------------------------------------

// Static prototypes ---------------------------------------------------------

static frames_repo_status frames_storage_save(
  const uint32_t frame_index,
  const frames_repo_data *const data
);
static frames_repo_status frames_storage_load(
  const uint32_t frame_index,
  frames_repo_data *const data
);
static uint32_t frames_storage_calculate_address(const uint32_t frame_index);

// Implementations -----------------------------------------------------------

void frames_storage_create()
{
  frames_repo_create(
    (frames_repo_io) {
      .load = frames_storage_load,
      .save = frames_storage_save
    }
  );
}

void frames_storage_destroy()
{
  frames_repo_destroy();
}

static frames_repo_status frames_storage_save(
  const uint32_t frame_index,
  const frames_repo_data *const data
)
{
  // status |= cy15b104q_driver_write_memory_data(
  //   (cy15b104q_driver_address) {
  //     .full = frames_storage_calculate_address(frame_index)
  //   },
  //   (uint8_t*)data,
  //   sizeof(frames_repo_data)
  // );

  uint32_t frame_address = frames_storage_calculate_address(frame_index);

  cy15b104q_driver_status status = cy15b104q_driver_write_enable();
  status |= cy15b104q_driver_write_memory_data(
    (cy15b104q_driver_address) {
      .full = frame_address
    },
    &data->start_byte,
    sizeof(uint8_t)
  );
  status |= cy15b104q_driver_write_enable();
  status |= cy15b104q_driver_write_memory_data(
    (cy15b104q_driver_address) {
      .full = frame_address + sizeof(uint8_t)
    },
    data->data,
    FRAMES_REPO_FRAME_DATA_SIZE
  );

  return (frames_repo_status)status;
}

static frames_repo_status frames_storage_load(
  const uint32_t frame_index,
  frames_repo_data *const data
)
{
  // cy15b104q_driver_status status = cy15b104q_driver_read_memory_data(
  //   (cy15b104q_driver_address) {
  //     .full = frames_storage_calculate_address(frame_index)
  //   },
  //   (uint8_t*)data,
  //   sizeof(frames_repo_data)
  // );

  uint32_t frame_address = frames_storage_calculate_address(frame_index);

  cy15b104q_driver_status status = cy15b104q_driver_read_memory_data(
    (cy15b104q_driver_address) {
      .full = frame_address
    },
    &data->start_byte,
    sizeof(uint8_t)
  );
  status |= cy15b104q_driver_read_memory_data(
    (cy15b104q_driver_address) {
      .full = frame_address + sizeof(uint8_t)
    },
    data->data,
    FRAMES_REPO_FRAME_DATA_SIZE
  );

  return (frames_repo_status)status;
}

static uint32_t frames_storage_calculate_address(const uint32_t frame_index)
{
  return FRAMES_STORAGE_START_ADDRESS +
    (frame_index * FRAMES_REPO_FRAME_SIZE);
}
