#ifndef LIST_OF_CHANGES_h
#define LIST_OF_CHANGES_h

#include <stdint.h>
#include <stdbool.h>
#include "led_panels_defs.h"

typedef struct
{
  uint8_t panel_position;
  uint8_t x;
  uint8_t y;
  led_panels_color color;
} pixel_change;

void list_of_changes_create(uint16_t size);
bool list_of_changes_add(pixel_change change);
bool list_of_changes_get(pixel_change *change);
void list_of_changes_clear(void);
void list_of_changes_destroy(void);

#endif
