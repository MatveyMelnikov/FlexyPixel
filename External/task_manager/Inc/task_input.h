#ifndef TASK_INPUT_H
#define TASK_INPUT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  uint8_t *data;
  bool is_input_changed;
} task_input;

#endif
