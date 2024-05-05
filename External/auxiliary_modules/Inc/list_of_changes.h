#ifndef LIST_OF_CHANGES_h
#define LIST_OF_CHANGES_h

#include <stdint.h>
#include <stdbool.h>
#include "led_panels_defs.h"

typedef enum {
  LIST_OF_CHANGES_OK = 0x0U,
  LIST_OF_CHANGES_IN_PROGRESS = 0x01U,
  LIST_OF_CHANGES_ERROR = 0x02U
} list_of_changes_status;

list_of_changes_status list_of_changes_load(void);
list_of_changes_status list_of_changes_save(void);
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
// Used to speed up rendering when a change occurs
bool list_of_changes_is_updated(void);
// Indicates the need to save changes to permanent memory
bool list_of_changes_is_need_to_save(void);
void list_of_changes_clear(void);

#endif
