#ifndef DEBUG_OUTPUT_H
#define DEBUG_OUTPUT_H

#include <stdint.h>

enum
{
  DEBUG_TIMEOUT = 100U,
  OUTPUT_BUFFER_SIZE = 200U,
  INTERNAL_BUFFER_SIZE = 100U
};

void debug_output_send(
  char *msg,
  uint16_t len,
  uint16_t line,
  char *file
);

#define DEBUG_OUTPUT(str, len) \
  debug_output_send(str, len, __LINE__, __FILE__);

#endif
