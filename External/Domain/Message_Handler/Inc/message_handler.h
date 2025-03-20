#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include "message_handler_defs.h"

void message_handler_set(const char *const message);
void message_handler_unset(void);
bool message_handler_is_first_field_equal(const char *const str);
bool message_handler_is_part_equal(
  const uint8_t start,
  const char *const str
);
char *const message_handler_get_part(const uint8_t start, const uint8_t len);

#endif
