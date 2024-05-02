#ifndef LIST_OF_CHANGES_h
#define LIST_OF_CHANGES_h

#include <stdint.h>
#include <stdbool.h>
#include "led_panels_defs.h"

bool list_of_changes_add(
  uint8_t panel_index,
  uint8_t pixel_index,
  led_panels_color color
);
void list_of_changes_apply_changes(led_panels_buffer *const target);
void list_of_changes_apply_raw_changes(
  uint8_t *const target,
  uint16_t target_len
);
bool list_of_changes_is_updated(void);
bool list_of_changes_are_there_any_changes(void);
void list_of_changes_clear(void);

#endif
