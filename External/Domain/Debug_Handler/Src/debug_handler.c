#include "debug_handler.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>

// Static variables ----------------------------------------------------------

static debug_handler_io debug_io;
static char *output_format = "-(%s, %d): %s\r\n";
static char output_buffer[DEBUG_HANDLER_OUTPUT_BUFFER_SIZE];
static char internal_buffer[DEBUG_HANDLER_INTERNAL_BUFFER_SIZE];

// Implementations -----------------------------------------------------------

void debug_output_create(debug_handler_io io)
{
  debug_io = io;
  debug_io.reset(true);
  debug_io.reset(false);
  debug_io.wait_before_connection();
}

void debug_output_destroy()
{
  debug_io.reset(true);
  debug_io = (debug_handler_io) { 0 };
}

debug_handler_status debug_output_send(
  char *msg,
  uint16_t len,
  uint16_t line,
  char *file
)
{
  if (len > DEBUG_HANDLER_INTERNAL_BUFFER_SIZE)
    len = DEBUG_HANDLER_INTERNAL_BUFFER_SIZE - 1;

  memcpy(internal_buffer, msg, len);
  *(internal_buffer + len) = '\0';
  sprintf(output_buffer, output_format, file, line, internal_buffer);

  return debug_io.output((uint8_t*)output_buffer, strlen(output_buffer));
}

debug_handler_status debug_output_with_insert_send(
  char *format,
  char *insertion,
  uint16_t line,
  char *file
)
{
  sprintf(internal_buffer, format, insertion);
  sprintf(output_buffer, output_format, file, line, internal_buffer);

  return debug_io.output((uint8_t*)output_buffer, strlen(output_buffer));
}
