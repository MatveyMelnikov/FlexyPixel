#include "image_mode_handler.h"
#include "hc06_driver.h"
#include "render_controller_defs.h"
#include "handler_queue.h"
#include "led_panels_driver.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// Static functions ----------------------------------------------------------

  // void (*receive)(mode_handler, led_panels_buffer *const);
  // void (*destroy)(mode_handler);

//static void 

static void handle_pixel_data(
  uint8_t *const data,
  led_panels_buffer *const buffer
)
{
  // Symbols: 50
  // {"panelNum": 9,"x": "00","y": "00","pixel": "967"}

  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  // Need to add auxiliary functions for parsing
  //uint8_t panel_num = 

  //led_panels_set_pixel(buffer, ) 
}

void set_handlers(mode_handler self, uint8_t *buffer)
{
  hc06_write((uint8_t *)OK_STRING, strlen(OK_STRING));

  //handle_function = handle_pixel_data;
  handler_queue_clear();
  handler_queue_add(handle_pixel_data);

  hc06_read(buffer, 50);
}

void destroy(mode_handler self)
{
  free(self);
}

// Static variables ----------------------------------------------------------

static mode_handler_interface_struct interface =
{
  set_handlers,
  destroy
};

// Implementations -----------------------------------------------------------

mode_handler image_mode_handler_create()
{
  mode_handler handler = malloc(sizeof(mode_handler_struct));
  handler->mode_name = "IMG";
  handler->render_delay = 500U;
  handler->vtable = &interface;

  return handler;
}