#ifndef DEBUG_HANDLER_DEFS_H
#define DEBUG_HANDLER_DEFS_H

#include <stdint.h>
#include <stdbool.h>

#define DEBUG_HANDLER_ON

enum
{
  DEBUG_HANDLER_OUTPUT_BUFFER_SIZE = 200U,
  DEBUG_HANDLER_INTERNAL_BUFFER_SIZE = 100U
};

typedef enum
{
  DEBUG_HANDLER_OK = 0U,
  DEBUG_HANDLER_BUSY,
  DEBUG_HANDLER_ERROR,
} debug_handler_status;

typedef struct
{
  // data, len
  debug_handler_status (*output)(uint8_t *, uint16_t);
  // is_reset
  void (*reset)(bool);
  // delay with checking host connection (max - 100ms)
  void (*wait_before_connection)(void);
  void (*delay)(uint32_t);
} debug_handler_io;

#endif
