#ifndef SINGLE_CHANGES_REPO_H
#define SINGLE_CHANGES_REPO_H

#include "single_changes_repo_defs.h"

void single_changes_repo_create(single_changes_repo_io io);
void single_changes_repo_destroy(void);
const single_changes_repo_external_data *const single_changes_repo_get(void);
void single_changes_repo_reset(void);
void single_changes_repo_set( 
  uint8_t panel_index,
  uint8_t pixel_index,
  led_panels_color color
);
single_changes_repo_status single_changes_repo_save(void);

#endif
