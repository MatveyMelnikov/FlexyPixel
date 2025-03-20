#ifndef DISPLAYS_CONFIG_REPO_DEFS_H
#define DISPLAYS_CONFIG_REPO_DEFS_H

#include <stdint.h>

typedef enum
{
  DISPLAYS_CONFIG_REPO_UNKNOWN = 0U,
  DISPLAYS_CONFIG_REPO_SMALL = 64U,
  DISPLAYS_CONFIG_REPO_BIG = 256U
} displays_config_repo_size;

enum {
  DISPLAYS_CONFIG_REPO_START_BYTE = 0xaaU,
  DISPLAYS_CONFIG_REPO_DISPLAYS_MAX_AMOUNT = 9U
};

typedef enum
{
  DISPLAYS_CONFIG_REPO_OK = 0U,
  DISPLAYS_CONFIG_REPO_ERROR,
  DISPLAYS_CONFIG_REPO_INVALID
} displays_config_repo_status;

typedef struct __attribute__((__packed__))
{
  uint8_t start_byte;
  uint32_t render_delay;
  uint16_t frames_amount;
  uint8_t configuration[9];
} displays_config_repo_internal_data;

typedef struct
{
  uint32_t render_delay;
  uint16_t frames_amount;
  displays_config_repo_size configuration[
    DISPLAYS_CONFIG_REPO_DISPLAYS_MAX_AMOUNT
  ];
  uint8_t displays_amount;
} displays_config_repo_external_data;

typedef struct
{
  // data
  displays_config_repo_status (*save)(
    const displays_config_repo_internal_data *const
  );
  // data
  displays_config_repo_status (*load)(
    displays_config_repo_internal_data *const
  );
} displays_config_repo_io;

#endif
