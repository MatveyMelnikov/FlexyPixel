#include "render_controller.h"
#include <render_controller_defs.h>
#include "led_panels_driver.h"
#include "displays_config_repo.h"
#include "frames_repo.h"
#include "single_changes_repo.h"
#include <stddef.h>

// Static variables ----------------------------------------------------------

static led_panels_buffer *front_buffer = NULL;
static led_panels_buffer *back_buffer = NULL;
static uint8_t displays_amount = 0U;
static uint16_t frames_amount = 0U;
static uint16_t frame_index = 0U;
static bool is_render_paused = false;

// Static prototypes ---------------------------------------------------------

static void render_controller_swap_buffers(void);
static render_controller_status render_controller_load_frame(void);
static void render_controller_apply_changes(void);

// Implementations -----------------------------------------------------------

void render_controller_create(void)
{
  render_controller_reset();
  render_controller_update_config();
}

void render_controller_update_config()
{
  const displays_config_repo_external_data *config = 
    displays_config_repo_get();

  if (config == NULL)
    return;
  displays_amount = config->displays_amount;
  frames_amount = config->frames_amount;

  led_panels_destroy(front_buffer);
  led_panels_destroy(back_buffer);

  front_buffer = led_panels_create(
    config->displays_amount,
    (led_panels_size*)&config->configuration
  );
  back_buffer = led_panels_create(
    config->displays_amount,
    (led_panels_size*)&config->configuration
  );

  (void)render_controller_load_frame();
  (void)render_controller_apply_changes();
}

void render_controller_destroy()
{
  led_panels_destroy(front_buffer);
  led_panels_destroy(back_buffer);
  render_controller_reset();
}

void render_controller_reset()
{
  led_panels_destroy(front_buffer);
  led_panels_destroy(back_buffer);

  displays_amount = 0U;
  frames_amount = 0U;
  frame_index = 0U;
  is_render_paused = false;
}

render_controller_status render_controller_render()
{
  if (front_buffer->is_locking | back_buffer->is_locking)
    return RENDER_CONTROLLER_STATUS_BUSY;
  if (is_render_paused)
    return RENDER_CONTROLLER_STATUS_OK;

  render_controller_swap_buffers();

  render_controller_status status = (render_controller_status)
    led_panels_send(front_buffer);
  status |= render_controller_load_frame();
  render_controller_apply_changes();

  return status;
}

static void render_controller_swap_buffers()
{
  led_panels_buffer *tmp = front_buffer;
  front_buffer = back_buffer;
  back_buffer = tmp;
}

static render_controller_status render_controller_load_frame()
{
  if (back_buffer == NULL)
    return RENDER_CONTROLLER_STATUS_ERROR;

  frames_repo_status status = frames_repo_get(
    frame_index,
    back_buffer->pixel_data,
    back_buffer->pixel_data_size
  );

  if (status)
    return RENDER_CONTROLLER_STATUS_ERROR;
  if (++frame_index >= frames_amount)
    frame_index = 0;

  return RENDER_CONTROLLER_STATUS_OK;
}

static void render_controller_apply_changes(void)
{
  const single_changes_repo_external_data *changes =
    single_changes_repo_get();

  single_changes_repo_external_change change = { 0 };
  for (
    uint8_t display_index = 0;
    display_index < displays_amount;
    display_index++
  )
  {
    for (
      uint8_t pixel_index = 0;
      pixel_index < LED_PANELS_SIZE_64;
      pixel_index++
    )
    {
      change = changes->changes[display_index][pixel_index];
      if (!change.is_active)
        continue;

      led_panels_set_pixel(
        back_buffer,
        display_index,
        pixel_index % 8U,
        pixel_index / 8U,
        change.color
      );
    }
  }
}

void render_controller_pause(bool is_paused)
{
  is_render_paused = is_paused;
}
