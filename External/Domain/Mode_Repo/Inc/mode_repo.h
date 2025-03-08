#ifndef MODE_REPO_H
#define MODE_REPO_H

#include "mode_repo_defs.h"
#include <stdbool.h>

void mode_repo_reset(void);
bool mode_repo_set(const char *const mode_str);
const char *const mode_repo_get(void);
bool mode_is_mode_set(void);

#endif
