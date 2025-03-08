#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdint.h>
#include <stdbool.h>
#include "hash_table_defs.h"

hash_table hash_table_create(
  uint8_t *const space,
  uint32_t size
);
void hash_table_destroy(hash_table *const self);
hash_table_status hash_table_insert(
  hash_table *const self,
  hash_table_item new_item
);
hash_table_item *hash_table_get(
  hash_table *const self,
  char *key
);

#endif
