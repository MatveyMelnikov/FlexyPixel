#include "hc06_driver.h"
#include "hc06_defs.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

static hc06_io module_io;
static char buffer[HC06_BUFFER_SIZE];
static char *baudrate_cmd = "AT+BAUD%u";
static char *change_name_cmd = "AT+NAME%s";
static char *change_pin_cmd = "AT+PIN%04u";
static const uint32_t baudrate_int[] = {
  1200, 2400, 4800, 9600, 19200, 38400, 
  57600, 115200, 230400, 460800
};
static hc06_baudrate current_baudrate;
static volatile bool is_data_receiving = false;

// Defines -------------------------------------------------------------------

#define GET_BAUDRATE_INT(baudrate_enum) \
  baudrate_int[(baudrate_enum)]

// Static functions ----------------------------------------------------------

static void set_check_cmd()
{
  strcpy(buffer, "AT");
}

static void set_baudrate_cmd(hc06_baudrate baudrate)
{
  // sprintf(buffer, baudrate_cmd, GET_BAUDRATE_INT(baudrate));
  sprintf(buffer, baudrate_cmd, (uint32_t)(baudrate) + 1U);
}

static void set_change_name_cmd(const char *const name)
{
  sprintf(buffer, change_name_cmd, name);
}

static void set_change_pin_cmd(const uint16_t pin)
{
  sprintf(buffer, change_pin_cmd, pin);
}

static hc06_status send_at_cmd()
{
  hc06_status status = module_io.write((uint8_t*)buffer, strlen(buffer));
  status |= module_io.blocking_read((uint8_t*)buffer, 2U);

  if (strstr(buffer, "OK") == NULL)
    return HC06_ERROR;

  module_io.delay(HC06_AT_DELAY);

  return status | HC06_OK;
}

// Implementations -----------------------------------------------------------

void hc06_create(hc06_io io)
{
  module_io = io;

  hc06_set_baudrate(HC06_9600);
}

void hc06_destroy()
{
  module_io.set_controller_baudrate(GET_BAUDRATE_INT(HC06_9600));
  module_io = (hc06_io) { 0 };
}

hc06_status hc06_check_link()
{
  set_check_cmd();
  (void)send_at_cmd(); // Dummy AT cmd to wake up HC-06
  set_check_cmd();
  return send_at_cmd();
}

hc06_status hc06_set_baudrate(hc06_baudrate baudrate)
{
  (void)hc_06_determine_baudrate();
  set_baudrate_cmd(baudrate);

  hc06_status status = send_at_cmd();
  status |= module_io.set_controller_baudrate(GET_BAUDRATE_INT(baudrate));
  current_baudrate = baudrate;

  return status;
}

hc06_baudrate hc_06_determine_baudrate(void)
{
  hc06_baudrate baudrate = HC06_1200;
  for (; baudrate <= HC06_UNDEFINED; baudrate++)
  {
    (void)module_io.set_controller_baudrate(GET_BAUDRATE_INT(baudrate));
    // (void)hc06_check_link(); // dummy cmd to wakeup hc06
    if (hc06_check_link() == HC06_OK)
      break;
  }

  return baudrate;
}

// The name is limited in 20 characters
// (Guangzhou HC IT HC-06 product datasheet pg. 16).
hc06_status hc06_set_name(const char* const name)
{
  if (strlen(name) > HC06_MAX_NAME_LEN)
    return HC06_ERROR;

  set_change_name_cmd(name);

  return send_at_cmd();
}

hc06_status hc06_set_pin(const uint16_t pin)
{
  if (pin > HC06_MAX_PIN_CODE)
    return HC06_ERROR;

  set_change_pin_cmd(pin);

  return send_at_cmd();
}

hc06_status hc06_write(const uint8_t *const data, const uint16_t size)
{
  return module_io.write(data, size);
}

hc06_status hc06_read(uint8_t *const data, const uint16_t size)
{
  if (is_data_receiving)
    return HC06_BUSY;

  hc06_status status = module_io.non_blocking_read(data, size);
  if (status)
    return status;

  is_data_receiving = true;
  return status;
}

void hc06_receive_complete(void)
{
  is_data_receiving = false;
}

bool hc06_is_data_received(void)
{
  return !is_data_receiving;
}
