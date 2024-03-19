#include "displays_conf.h"

// Static variables ----------------------------------------------------------

static led_panels_size display_configuration[CONFIGURATION_SIZE];
static bool is_updated = false;
static uint8_t displays_num = 0;

// Implementations -----------------------------------------------------------

void displays_conf_update(
  led_panels_size *const new_configuration,
  uint8_t num
)
{
  displays_num = num;

  memcpy(
    display_configuration,
    new_configuration,
    sizeof(led_panels_size) * displays_num
  );

  is_updated = true;
}

bool displays_conf_is_updated()
{
  return is_updated;
}

void displays_conf_reset_update_flag()
{
  is_updated = false;
}

bool displays_conf_is_empty()
{
  return display_configuration[0] == 0;
}

void displays_conf_clear()
{
  memset(
    display_configuration,
    0,
    sizeof(led_panels_size) * CONFIGURATION_SIZE
  );
  displays_num = 0;
}

uint16_t displays_conf_get_pixels_num()
{
  uint16_t result = 0;
  for (uint8_t i = 0; i < displays_num; i++)
    result += (uint16_t)display_configuration[i];

  return result;
}

uint8_t displays_conf_get_displays_num()
{
  return displays_num;
}

led_panels_size *const displays_conf_get()
{
  return display_configuration;
}

bool displays_conf_is_panel_configured(uint8_t panel_num)
{
  return display_configuration[panel_num] != LED_PANELS_UNKNOWN;
}
