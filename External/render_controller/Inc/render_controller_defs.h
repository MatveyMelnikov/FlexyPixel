#ifndef RENDER_CONTROLLER_DEFS_H
#define RENDER_CONTROLLER_DEFS_H

#include <stdint.h>

enum
{
  MODES_NUM = 1U,
  INPUT_BUFFER_SIZE = 500U
};

#define OK_STRING "{\"status\":\"OK\"}"
#define ERROR_STRING "{\"status\":\"ERROR\"}"
#define UNCONFIGURED_STRING "{\"status\":\"UNCONFIGURED\"}"

#define STR_TO_NUM(str) \
  ((uint16_t)(*(str)) - '0') * 100 + \
  ((uint16_t)(*((str) + 1)) - '0') * 10 + \
  ((uint16_t)(*((str) + 2)) - '0')

#define GET_SIDE_SIZE(panel_size) \
  

#endif
