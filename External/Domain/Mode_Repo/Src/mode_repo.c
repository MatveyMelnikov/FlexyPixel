#include "mode_repo.h"
#include <string.h>
#include <stddef.h>

// Static variables ----------------------------------------------------------

static char current_mode[4];
static bool is_mode_set = false;

// Static prototypes ---------------------------------------------------------

static bool mode_repo_is_invalid_mode(const char *const mode_str);

// Implementations -----------------------------------------------------------

void mode_repo_reset()
{
  is_mode_set = false;
}

bool mode_repo_set(const char *const mode_str)
{
  if (mode_repo_is_invalid_mode(mode_str))
    return false;

  memcpy(current_mode, mode_str, 3U);
  current_mode[3] = '\0';

  is_mode_set = true;
  return true;
}

static bool mode_repo_is_invalid_mode(const char *const mode_str)
{
  return (
    (bool)memcmp(mode_str, MODE_REPO_SEQ, 3U) &&
    (bool)memcmp(mode_str, MODE_REPO_PIX, 3U)
  );
}

const char *const mode_repo_get()
{
  if (!is_mode_set)
    return NULL;

  return current_mode;
}

bool mode_is_mode_set()
{
  return is_mode_set;
}
