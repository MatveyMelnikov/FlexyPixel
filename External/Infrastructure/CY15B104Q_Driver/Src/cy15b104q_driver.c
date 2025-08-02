#include "cy15b104q_driver.h"
#include <stddef.h>

// Defines -------------------------------------------------------------------

#define BOOL_TO_BIT(boolean_value) \
  (boolean_value) ? 1 : 0

#define BOOL_TO_ABSOLUTE(boolean_value) \
  ((boolean_value) ? ~0 : 0)

// Static variables ----------------------------------------------------------

static cy15b104q_driver_io_struct module_io;

// Static prototypes ---------------------------------------------------------

static cy15b104q_driver_status cy15b104q_driver_dummy_read(void);
static uint8_t cy15b104q_driver_create_new_status_register(
  bool write_protect_enabled,
  bool block_protect_0,
  bool block_protect_1
);
__attribute__((always_inline))
static inline cy15b104q_driver_address cy15b104q_reverse_address(
  cy15b104q_driver_address address
);
__attribute__((always_inline))
static inline bool cy15b104q_is_address_out_of_bounds(
  cy15b104q_driver_address address,
  const uint16_t data_size
);
__attribute__((always_inline))
static inline cy15b104q_driver_status cy15b104q_driver_io_transmit(
  const uint8_t *const data,
  const uint16_t size
);
__attribute__((always_inline))
static inline cy15b104q_driver_status cy15b104q_driver_io_receive(
  uint8_t *const data,
  const uint16_t size
);
__attribute__((always_inline))
static inline cy15b104q_driver_status cy15b104q_driver_io_write_cs_pin(
  const bool is_set
);
__attribute__((always_inline))
static inline cy15b104q_driver_status cy15b104q_driver_io_delay(
  uint32_t timeout
);

// Implementations -----------------------------------------------------------

void cy15b104q_driver_init_module(
  cy15b104q_driver_io_struct io_struct
)
{
  module_io = io_struct;
}

void cy15b104q_driver_deinit_module()
{
  module_io = (cy15b104q_driver_io_struct) { 0 };
}

cy15b104q_driver_status cy15b104q_driver_power_up()
{
  // CY15B104Q datasheet, pg. 6
  cy15b104q_driver_status status = cy15b104q_driver_io_write_cs_pin(true);
  status |= cy15b104q_driver_io_delay(CY15B104Q_POWER_UP_DELAY);
  status |= cy15b104q_driver_dummy_read();
  status |= cy15b104q_driver_io_delay(CY15B104Q_POWER_UP_DELAY);

  return status;
}

static cy15b104q_driver_status cy15b104q_driver_dummy_read()
{
  uint8_t dummy_data;
  cy15b104q_driver_status status = cy15b104q_driver_read_status_register(
    &dummy_data
  );

  return status;
}

cy15b104q_driver_status cy15b104q_driver_check_link(void)
{
  cy15b104q_driver_id id;

  cy15b104q_driver_status status = cy15b104q_driver_read_id(&id);
  if (status || *(uint16_t*)id.product_id == CY15B104Q_PRODUCT_ID)
    return status;
  
  return status | CY15B104Q_STATUS_ERROR;
}

cy15b104q_driver_status cy15b104q_driver_read_id(
  cy15b104q_driver_id *const result_id
)
{
  // CY15B104Q datasheet, pg. 11
  uint8_t data_out = CY15B104Q_CMD_ID;

  cy15b104q_driver_status status = cy15b104q_driver_io_write_cs_pin(false);
  status |= cy15b104q_driver_io_transmit(&data_out, sizeof(uint8_t));
  status |= cy15b104q_driver_io_receive(
    (uint8_t*)result_id,
    sizeof(cy15b104q_driver_id)
  );
  status |= cy15b104q_driver_io_write_cs_pin(true);

  return status;
}

cy15b104q_driver_status cy15b104q_driver_read_status_register(
  uint8_t *const result_status_register
)
{
  uint8_t cmd = CY15B104Q_CMD_READ_STATUS;

  cy15b104q_driver_status status = cy15b104q_driver_io_write_cs_pin(false);
  status |= cy15b104q_driver_io_transmit(&cmd, sizeof(uint8_t));
  status |= cy15b104q_driver_io_receive(
    result_status_register,
    sizeof(uint8_t)
  );
  status |= cy15b104q_driver_io_write_cs_pin(true);

  return status;
}

cy15b104q_driver_status cy15b104q_driver_write_status_register(
  bool write_protect_enabled,
  bool block_protect_0,
  bool block_protect_1
)
{
  uint8_t cmd = CY15B104Q_CMD_WRITE_STATUS;
  uint8_t new_status_register = cy15b104q_driver_create_new_status_register(
    write_protect_enabled,
    block_protect_0,
    block_protect_1
  );

  cy15b104q_driver_status status = cy15b104q_driver_io_write_cs_pin(false);
  status |= cy15b104q_driver_io_transmit(&cmd, sizeof(uint8_t));
  status |= cy15b104q_driver_io_transmit(
    &new_status_register,
    sizeof(uint8_t)
  );
  status |= cy15b104q_driver_io_write_cs_pin(true);

  return status;
}

static uint8_t cy15b104q_driver_create_new_status_register(
  bool write_protect_enabled,
  bool block_protect_0,
  bool block_protect_1
)
{
  uint8_t is_write_protect_enabled = BOOL_TO_ABSOLUTE(write_protect_enabled);
  uint8_t is_block_0_protect = BOOL_TO_ABSOLUTE(block_protect_1);
  uint8_t is_block_1_protect = BOOL_TO_ABSOLUTE(block_protect_0);

  return (is_write_protect_enabled & CY15B104Q_STATUS_REG_WPEN) |
    (is_block_0_protect & CY15B104Q_STATUS_REG_BP1) |
    (is_block_1_protect & CY15B104Q_STATUS_REG_BP0);
}

cy15b104q_driver_status cy15b104q_driver_write_enable()
{
  uint8_t cmd = CY15B104Q_CMD_WRITE_ENABLE;

  cy15b104q_driver_status status = cy15b104q_driver_io_write_cs_pin(false);
  status |= cy15b104q_driver_io_transmit(&cmd, sizeof(uint8_t));
  status |= cy15b104q_driver_io_write_cs_pin(true);

  return status;
}

cy15b104q_driver_status cy15b104q_driver_write_disable()
{
  uint8_t cmd = CY15B104Q_CMD_WRITE_DISABLE;

  cy15b104q_driver_status status = cy15b104q_driver_io_write_cs_pin(false);
  status |= cy15b104q_driver_io_transmit(&cmd, sizeof(uint8_t));
  status |= cy15b104q_driver_io_write_cs_pin(true);

  return status;
}

cy15b104q_driver_status cy15b104q_driver_write_memory_data(
  const cy15b104q_driver_address addr,
  const uint8_t *const data,
  const uint16_t size
)
{
  uint8_t cmd = CY15B104Q_CMD_WRITE_MEMORY_DATA;

  if (cy15b104q_is_address_out_of_bounds(addr, size))
    return CY15B104Q_STATUS_ERROR;

  cy15b104q_driver_address reversed_addr = cy15b104q_reverse_address(addr);

  cy15b104q_driver_status status = cy15b104q_driver_io_write_cs_pin(false);
  status |= cy15b104q_driver_io_transmit(&cmd, sizeof(uint8_t));
  status |= cy15b104q_driver_io_transmit(
    reversed_addr.used_parts,
    sizeof(reversed_addr.used_parts)
  );
  status |= cy15b104q_driver_io_transmit(data, size);
  status |= cy15b104q_driver_io_write_cs_pin(true);

  return status; 
}

cy15b104q_driver_status cy15b104q_driver_read_memory_data(
  const cy15b104q_driver_address addr,
  uint8_t *const data,
  const uint16_t size
)
{
  uint8_t cmd = CY15B104Q_CMD_READ_MEMORY_DATA;

  if (cy15b104q_is_address_out_of_bounds(addr, size))
    return CY15B104Q_STATUS_ERROR;

  cy15b104q_driver_address reversed_addr = cy15b104q_reverse_address(addr);

  cy15b104q_driver_status status = cy15b104q_driver_io_write_cs_pin(false);
  status |= cy15b104q_driver_io_transmit(&cmd, sizeof(uint8_t));
  status |= cy15b104q_driver_io_transmit(
    reversed_addr.used_parts,
    sizeof(reversed_addr.used_parts)
  );
  status |= cy15b104q_driver_io_receive(data, size);
  status |= cy15b104q_driver_io_write_cs_pin(true);

  return status; 
}

__attribute__((always_inline))
static inline cy15b104q_driver_address cy15b104q_reverse_address(
  cy15b104q_driver_address address
)
{
  return (cy15b104q_driver_address) {
    .used_parts = {
      address.used_parts[2], address.used_parts[1], address.used_parts[0]
    }
  };
}

__attribute__((always_inline))
static inline bool cy15b104q_is_address_out_of_bounds(
  cy15b104q_driver_address address,
  const uint16_t data_size
)
{
  if (address.full + data_size >= CY15B104Q_SIZE)
    return true;

  return false;
}

__attribute__((always_inline))
static inline cy15b104q_driver_status cy15b104q_driver_io_transmit(
  const uint8_t *const data,
  const uint16_t size
)
{
  if (module_io.transmit == NULL)
    return CY15B104Q_STATUS_IS_NOT_INIT;

  return module_io.transmit(data, size, CY15B104Q_IO_TIMEOUT);
}

__attribute__((always_inline))
static inline cy15b104q_driver_status cy15b104q_driver_io_receive(
  uint8_t *const data,
  const uint16_t size
)
{
  if (module_io.receive == NULL)
    return CY15B104Q_STATUS_IS_NOT_INIT;

  return module_io.receive(data, size, CY15B104Q_IO_TIMEOUT);
}

__attribute__((always_inline))
static inline cy15b104q_driver_status cy15b104q_driver_io_write_cs_pin(
  const bool is_set
)
{
  if (module_io.write_cs_pin == NULL)
    return CY15B104Q_STATUS_IS_NOT_INIT;
  
  module_io.write_cs_pin(is_set);

  return CY15B104Q_STATUS_OK;
}

__attribute__((always_inline))
static inline cy15b104q_driver_status cy15b104q_driver_io_delay(
  uint32_t timeout
)
{
  if (module_io.delay == NULL)
    return CY15B104Q_STATUS_IS_NOT_INIT;

  module_io.delay(timeout);

  return CY15B104Q_STATUS_OK;
}
