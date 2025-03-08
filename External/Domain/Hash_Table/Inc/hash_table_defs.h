#ifndef HASH_TABLE_DEFS_H
#define HASH_TABLE_DEFS_H

#include <stdint.h>

enum {
  HASH_TABLE_SEED = 0U,
};

typedef enum
{
  HASH_TABLE_STATUS_OK = 0U,
  HASH_TABLE_STATUS_ERROR,
} hash_table_status;

typedef struct
{
  char *key;
  void *ptr;
  uint32_t psl;
} hash_table_item;

typedef struct
{
  hash_table_item *items;
  uint32_t items_amount;
} hash_table;

#endif
