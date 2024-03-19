#include "list_of_changes.h"
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>

static pixel_change *list = NULL;
static uint16_t top_index = 0;
static uint16_t current_index = 0;
static uint16_t current_size = 0;

void list_of_changes_create(uint16_t size)
{
  if (list)
    list_of_changes_destroy();

  current_size = size;
  list = calloc(size, sizeof(pixel_change));
}

bool list_of_changes_add(pixel_change change)
{
  if (top_index >= current_size)
    return false;

  list[top_index++] = change;
  return true;
}

bool list_of_changes_get(pixel_change *change)
{
  if (current_index >= top_index)
    return false;
  
  *change = list[current_index++];
  return true;
}

void list_of_changes_clear()
{
  top_index = 0;
  current_index = 0;
}

void list_of_changes_destroy()
{
  if (list)
    free(list);
  list = NULL;
  list_of_changes_clear();
  current_size = 0;
}
