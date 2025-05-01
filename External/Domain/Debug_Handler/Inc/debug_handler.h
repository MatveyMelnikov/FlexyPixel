#ifndef DEBUG_HANDLER_H
#define DEBUG_HANDLER_H

#include <stdint.h>
#include <string.h>
#include "debug_handler_defs.h"

void debug_output_create(debug_handler_io io);
void debug_output_destroy(void);
debug_handler_status debug_output_send(
  char *msg,
  uint16_t len,
  uint16_t line,
  char *file
);

debug_handler_status debug_output_with_insert_send(
  char *format,
  char *insertion,
  uint16_t line,
  char *file
);

#ifdef DEBUG_HANDLER_ON

#define DEBUG_HANDLER_OUTPUT(str) \
  debug_output_send((str), strlen(str), (__LINE__), (__FILE__))

#define DEBUG_HANDLER_FORMAT_OUTPUT(format, insertion) \
  debug_output_with_insert_send((format), (insertion), (__LINE__), (__FILE__))

#else

#define DEBUG_OUTPUT(str) \
  (void)

#define DEBUG_OUTPUT_WITH_INSERT(format, insertion) \
  (void)

#endif

#endif
