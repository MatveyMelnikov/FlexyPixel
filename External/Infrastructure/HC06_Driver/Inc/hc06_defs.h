#ifndef HC06_DEFS_H
#define HC06_DEFS_H

enum
{
	HC06_TIMEOUT = 500U,
  HC06_AT_DELAY = 100U,
  HC06_BUFFER_SIZE = 40U,
  HC06_MAX_PIN_CODE = 9999U, // 4 digits
  HC06_MAX_NAME_LEN = 20U
};

typedef enum
{
  HC06_1200 = 0x0U,
  HC06_2400,
  HC06_4800,
  HC06_9600,
  HC06_19200,
  HC06_38400,
  HC06_57600,
  HC06_115200,
  HC06_230400,
  HC06_460800, // stm32f103 max baudrate is 500 kbits/s
  HC06_UNDEFINED
} hc06_baudrate;

typedef enum
{
  HC06_OK = 0x00U,
  HC06_ERROR = 0x01U,
  HC06_BUSY = 0x02U,
  HC06_TIMEOUT_ERROR = 0x03U,
  HC06_OVERFLOW = 0x04U
} hc06_status;

typedef struct
{
  // data, data_size
  hc06_status (*blocking_read)(uint8_t *const, const uint16_t);
  // data, data_size
  hc06_status (*write)(const uint8_t *const, const uint16_t);
  // data, data_size
  hc06_status (*non_blocking_read)(uint8_t *const, const uint16_t);
  // baudrate
  hc06_status (*set_controller_baudrate)(const uint32_t);
  // delay in ms
  void (*delay)(uint32_t);
} hc06_io;

#endif
