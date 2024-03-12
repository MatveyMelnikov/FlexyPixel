#ifndef HC06_DRIVER_H
#define HC06_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "hc06_defs.h"

void hc06_create(void);
void hc06_destroy(void);
hc06_status hc06_check_link(void);
hc06_status hc06_set_baudrate(hc06_baudrate baudrate);
hc06_baudrate hc_06_determine_baudrate(void);
hc06_status hc06_set_name(const char* const name);
hc06_status hc06_set_pin(const uint16_t pin);
hc06_status hc06_write(const uint8_t *const data, const uint16_t size);
hc06_status hc06_read(uint8_t *const data, const uint16_t size);
hc06_status hc06_receive_complete(void);
bool hc06_is_data_received(void);

#endif
