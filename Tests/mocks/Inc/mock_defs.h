#ifndef MOCK_DEFS_H
#define MOCK_DEFS_H

#include <stdint.h>

typedef struct
{
  int kind;
  const uint8_t *data;
  uint8_t data_size;
} expectation;

typedef struct
{
  const char *current_mock_name;
  expectation *expectations;
  uint8_t set_expectation_count;
  uint8_t get_expectation_count;
  uint8_t max_expectation_count;
} mock;

typedef enum
{
  READ,
  WRITE,
  ACTIVE,
  INACTIVE,
  GENERATE,
  DESTROY,
  CALCULATE,
  START,
  FINISH,
  CREATE,
  SET,
  GET,
  NO_EXPECTED_VALUE = -1
} expectation_type;

#define UNUSED(n) (void)(n)

#endif