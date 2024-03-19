#ifndef RENDER_CONTROLLER_DEFS_H
#define RENDER_CONTROLLER_DEFS_H

#include <stdint.h>
#include <string.h>
#include "hc06_driver.h"

enum
{
  MODES_NUM = 1U,
  INPUT_BUFFER_SIZE = 500U,
  FIRST_FIELD_OFFSET = 2U,
  CMD_TYPE_OFFSET = 9U,
  CMD_LEN = 15U,
  MODE_OFFSET = 9U,
  CONFIGURATION_OFFSET = 19U,
  MODE_LEN = 3U,
  RENDER_DELAY = 46U // 21 fps - 1 ms
};

#define OK_STRING "{\"status\":\"OK\"}"
#define ERROR_STRING "{\"status\":\"ERROR\"}"
#define UNCONFIGURED_STRING "{\"status\":\"UNCONFIGURED\"}"

#define STR_TO_NUM(str) \
  ((uint16_t)(*(str)) - '0') * 100 + \
  ((uint16_t)(*((str) + 1)) - '0') * 10 + \
  ((uint16_t)(*((str) + 2)) - '0')

#define CHECK_STR(str_a, str_b, size) \
  (memcmp((str_a), (str_b), (size) * sizeof(char)) == 0)

__attribute__((always_inline))
inline void send_status(bool is_ok)
{
  if (is_ok)
    hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));
  else
    hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
}

__attribute__((always_inline))
inline 
bool is_first_field_not_suitable(uint8_t *data, char *expected)
{
  // Check first field name
  if (!CHECK_STR(
    data + FIRST_FIELD_OFFSET, expected, strlen(expected)
  ))
  {
    hc06_write((uint8_t *)ERROR_STRING, strlen(ERROR_STRING));
    return true;
  }
  return false;
}

#endif
