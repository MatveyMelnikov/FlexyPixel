#ifndef DISPLAYS_CONFIG_REPO_H
#define DISPLAYS_CONFIG_REPO_H

#include "displays_config_repo_defs.h"

void displays_config_repo_create(displays_config_repo_io io);
void displays_config_repo_destroy(void);
const displays_config_repo_external_data *const displays_config_repo_get(void);
displays_config_repo_status displays_config_repo_save(
  const displays_config_repo_external_data *const new_config
);

#endif
