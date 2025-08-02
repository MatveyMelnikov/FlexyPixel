// Robin Hood Hashing

#include "hash_table.h"
#include <string.h>
#include <stdbool.h>

// Static prototypes ---------------------------------------------------------

static uint32_t hash_table_larson_hash(const char* key, uint32_t seed);
static bool hash_table_is_item_free(
  const hash_table_item *const item
);
static void hash_table_try_insert(
  hash_table *const self,
  uint32_t index,
  hash_table_item *const inserting_item
);
static bool hash_table_is_right_item(
  const char* key,
  const hash_table_item *const item
);

// Implementations -----------------------------------------------------------

hash_table hash_table_create(
  uint8_t *const space,
  uint32_t size
)
{
  memset(space, 0, size);

  return (hash_table) {
    .items = (hash_table_item*)space,
    .items_amount = size / sizeof(hash_table_item)
  };
}

void hash_table_destroy(hash_table *const self)
{
  self->items = NULL;
  self->items_amount = 0U;
}

hash_table_status hash_table_insert(
  hash_table *const self,
  hash_table_item new_item
)
{
  uint32_t index = hash_table_larson_hash(new_item.key, HASH_TABLE_SEED) %
    self->items_amount;
  new_item.psl = 0;

  if (hash_table_is_item_free(&self->items[index]))
  {
    self->items[index] = new_item;
    return HASH_TABLE_STATUS_OK;
  }

  hash_table_item current_item = new_item;
  for (
    uint32_t current_index = index;
    current_index < self->items_amount;
    current_index++
  )
  {
    hash_table_try_insert(
      self,
      current_index,
      &current_item
    );

    if (hash_table_is_item_free(&current_item))
      return HASH_TABLE_STATUS_OK;
  }

  return (hash_table_is_item_free(&current_item)) ?
    HASH_TABLE_STATUS_OK : HASH_TABLE_STATUS_ERROR;
}

static bool hash_table_is_item_free(
  const hash_table_item *const item
)
{
  return (item->key == NULL);
}

static void hash_table_try_insert(
  hash_table *const self,
  uint32_t index,
  hash_table_item *const inserting_item
)
{
  hash_table_item displaced_element = { 0 };

  if (self->items[index].psl >= inserting_item->psl)
  {
    inserting_item->psl++;
    return;
  }

  displaced_element = self->items[index];
  displaced_element.psl++;

  self->items[index] = *inserting_item;

  *inserting_item = displaced_element;
}

hash_table_item *hash_table_get(
  hash_table *const self,
  char *key
)
{
  uint32_t index = hash_table_larson_hash(key, HASH_TABLE_SEED) %
    self->items_amount;
  hash_table_item *found_item = &self->items[index];

  if (hash_table_is_right_item(key, found_item))
  {
    return found_item;
  }

  for (
    uint32_t current_index = index + 1;
    current_index < self->items_amount;
    current_index++
  )
  {
    found_item = &self->items[current_index];

    if (hash_table_is_right_item(key, found_item))
      return found_item;
  }

  return NULL;
}

static uint32_t hash_table_larson_hash(const char* key, uint32_t seed)
{
  uint32_t hash = seed;
  while (*key)
  {
    hash = hash * 101U  +  *(key++);
  }

  return hash;
}

static bool hash_table_is_right_item(
  const char* key,
  const hash_table_item *const item
)
{
  return (strcmp(key, item->key) == 0) ? true : false;
}
