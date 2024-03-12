#ifndef HC06_DEFS_H
#define HC06_DEFS_H

enum
{
	HC06_TIMEOUT = 500U,
  HC06_AT_DELAY = 20U,
  HC06_BUFFER_SIZE = 40U
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

#endif
