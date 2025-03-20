#ifndef SINGLE_CHANGES_REPO_DEFS_H
#define SINGLE_CHANGES_REPO_DEFS_H

#include <stdint.h>
#include <stdbool.h>
#include "led_panels_defs.h"

enum {
  SINGLE_CHANGES_REPO_START_BYTE = 0x55U,
  SINGLE_CHANGES_REPO_PANELS_AMOUNT = 9U,
  SINGLE_CHANGES_REPO_CHANGES_AMOUNT = LED_PANELS_SIZE_64 *
    SINGLE_CHANGES_REPO_PANELS_AMOUNT,
};

typedef enum
{
  SINGLE_CHANGES_REPO_OK = 0U,
  SINGLE_CHANGES_REPO_ERROR,
  SINGLE_CHANGES_REPO_INVALID
} single_changes_repo_status;

typedef struct __attribute__((__packed__)) // 2 bytes
{
  uint16_t pixel_color : 12;
  uint8_t is_active : 4;
} single_changes_repo_internal_change;

typedef struct __attribute__((__packed__)) // 16 bytes
{
  uint8_t start_byte;
  single_changes_repo_internal_change changes[
    SINGLE_CHANGES_REPO_CHANGES_AMOUNT
  ]; // 1.5 bytes per pixel + is_active
} single_changes_repo_internal_data;

typedef struct // 2 bytes
{
  led_panels_color color;
  bool is_active;
} single_changes_repo_external_change;

typedef struct
{
  single_changes_repo_external_change changes[9U][64U];
} single_changes_repo_external_data;

typedef struct
{
  // data
  single_changes_repo_status (*save)(
    const single_changes_repo_internal_data *const
  );
  // data
  single_changes_repo_status (*load)(
    single_changes_repo_internal_data *const
  );
} single_changes_repo_io;

#endif
