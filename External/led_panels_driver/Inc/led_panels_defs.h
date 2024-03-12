#ifndef LED_PANELS_DEFS_H
#define LED_PANELS_DEFS_H

#include <stdbool.h>

enum
{
  LED_PANELS_1_VALUE = 60U,
  LED_PANELS_0_VALUE = 30U,
  LED_PANELS_RESET_VALUE = 0U
};

typedef enum
{
  LED_PANELS_SIZE_64 = 64U,
  LED_PANELS_SIZE_256 = 256U
} led_panels_size;

typedef struct
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} led_panels_color;

typedef struct
{
  uint8_t *pwm_data;
  uint16_t panels_num;
  led_panels_size *panels_sizes;
  bool is_locking;
} led_panels_buffer;

typedef enum
{
  LED_PANELS_OK = 0x00U,
  LED_PANELS_ERROR = 0x01U,
  LED_PANELS_BUSY = 0x02U,
  LED_PANELS_TIMEOUT_ERROR = 0x03U,
  LED_PANELS_OVERFLOW = 0x04U,
  LED_PANELS_BOUNDS = 0x05U
} led_panels_status;

#endif
