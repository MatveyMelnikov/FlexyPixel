#ifndef DISPLAYS_CONF_H
#define DISPLAYS_CONF_H

#include "led_panels_defs.h"
#include <stdbool.h>
#include <stdint.h>

enum
{
  CONFIGURATION_SIZE = 9U
};

void displays_conf_update(
  led_panels_size *const new_configuration,
  uint8_t num
);
bool displays_conf_is_updated(void);
void displays_conf_reset_update_flag(void);
bool displays_conf_is_empty(void);
void displays_conf_clear(void);
uint16_t displays_conf_get_pixels_num(void);
uint8_t displays_conf_get_displays_num(void);
led_panels_size *const displays_conf_get(void);
bool displays_conf_is_panel_configured(uint8_t panel_num);

#endif
