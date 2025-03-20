#ifndef TASK_DEFS_H
#define TASK_DEFS_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
  TASK_STATUS_COMPLETED = 0U,
  TASK_STATUS_IN_PROGRESS,
  TASK_STATUS_TIMEOUT
} task_status;

typedef enum
{
  TASK_RESPONSE_OK = 0U,
  TASK_RESPONSE_ERROR,
  TASK_RESPONSE_UNCONFIGURED,
} task_response;

typedef struct
{
  task_status status;
  task_response response;
} task_output;

typedef struct
{
  uint8_t *data;
  uint32_t data_size;
  bool is_changed;
} task_data;

typedef struct
{
  task_data *input;
  task_data *output;
  uint32_t current_tick;
} task_arg;

#define TASK_OUTPUT_COMPLETED \
  (task_output) { \
    .response = TASK_RESPONSE_OK, \
    .status = TASK_STATUS_COMPLETED \
  }
#define TASK_OUTPUT_IN_PROGRESS \
  (task_output) { \
    .response = TASK_RESPONSE_OK, \
    .status = TASK_STATUS_IN_PROGRESS \
  }
#define TASK_OUTPUT_ERROR \
  (task_output) { \
    .response = TASK_RESPONSE_ERROR, \
    .status = TASK_STATUS_COMPLETED \
  }

#endif
