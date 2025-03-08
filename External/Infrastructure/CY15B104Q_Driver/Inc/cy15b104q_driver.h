#ifndef CY15B104Q_DRIVER_H
#define CY15B104Q_DRIVER_H

#include "cy15b104q_driver_defs.h"

void cy15b104q_driver_init_module(
  cy15b104q_driver_io_struct io_struct
);
void cy15b104q_driver_deinit_module(void);
cy15b104q_driver_status cy15b104q_driver_power_up(void);
cy15b104q_driver_status cy15b104q_driver_check_link(void);
cy15b104q_driver_status cy15b104q_driver_read_id(
  cy15b104q_driver_id *const result_id
);
cy15b104q_driver_status cy15b104q_driver_read_status_register(
  uint8_t *const result_status_register
);
cy15b104q_driver_status cy15b104q_driver_write_status_register(
  bool write_protect_enabled,
  bool block_protect_0,
  bool block_protect_1
);
cy15b104q_driver_status cy15b104q_driver_write_enable(void);
cy15b104q_driver_status cy15b104q_driver_write_disable(void);
cy15b104q_driver_status cy15b104q_driver_write_memory_data(
  const cy15b104q_driver_address addr,
  const uint8_t *const data,
  const uint16_t size
);
cy15b104q_driver_status cy15b104q_driver_read_memory_data(
  const cy15b104q_driver_address addr,
  uint8_t *const data,
  const uint16_t size
);

#endif
