#include "message_handler.h"
#include <stddef.h>
#include <string.h>

// Static variables ----------------------------------------------------------

const char *processed_message = NULL;
static char inner_buffer[MESSAGE_HANDLER_INNER_BUFFER_LEN];

// Static prototypes ---------------------------------------------------------

// Implementations -----------------------------------------------------------

void message_handler_set(const char *const message)
{
  processed_message = message;
}

void message_handler_unset()
{
  processed_message = NULL;
}

bool message_handler_is_first_field_equal(const char *const str)
{
  return message_handler_is_part_equal(
    MESSAGE_HANDLER_FIRST_FIELD_OFFSET,
    str
  );
}

bool message_handler_is_part_equal(
  const uint8_t start,
  const char *const str
)
{
  if (processed_message == NULL)
    return false;
  
  if (memcmp(processed_message + start, str, strlen(str)) == 0)
    return true;
  return false;
}

char *const message_handler_get_part(const uint8_t start, const uint8_t len)
{
  if (processed_message == NULL)
    return false;
  if ((len + 1U) >= MESSAGE_HANDLER_INNER_BUFFER_LEN)
    return NULL;

  memcpy(inner_buffer, processed_message + start, len);
  inner_buffer[len] = '\0';

  return inner_buffer;
}
