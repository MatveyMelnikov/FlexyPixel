#include "mock_flash_driver_io.h"
#include "unity_fixture.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
  int kind;
  const uint8_t * input_data;
  const uint8_t * output_data;
  uint16_t input_data_size;
  uint16_t output_data_size;
} expectation;

enum
{
  IO_WRITE,
  IO_WRITE_READ,
  IO_SELECT,
  IO_DISELECT,
  NO_EXPECTED_VALUE = -1
};

static char *report_not_init = "MockIO not initialized";
static char *report_no_room_for_exp = "No room for expectations in MockIO";
static char *report_verify_error = "Verify error in MockIO. Expected %u "
  "operations, but got %u";
static char *report_kind_error = "Error kind in MockIO."
  " Num of expectation %u ";
static char *report_addr_error = "Error addr in MockIO."
  " Num of expectation %u ";
static char *report_data_error = "Error data in MockIO."
  " Expected %u, but got %u";

static expectation *expectations = NULL;
static int set_expectation_count;
static int get_expectation_count;
static int max_expectation_count;

// Static functions ----------------------------------------------------------

static void fail_when_no_init()
{
  if (expectations == NULL)
    FAIL(report_not_init);
}

static void fail_when_no_room_for_expectations()
{
  fail_when_no_init();
  if (set_expectation_count >= max_expectation_count)
    FAIL(report_no_room_for_exp);
}


static void record_expectation(
  const int kind,
  const uint8_t *const input_data,
  const uint8_t *const output_data,
  const uint16_t input_data_size,
  const uint16_t output_data_size
)
{
  expectations[set_expectation_count].kind = kind;
  expectations[set_expectation_count].input_data = input_data;
  expectations[set_expectation_count].output_data = output_data;
  expectations[set_expectation_count].input_data_size = input_data_size;
  expectations[set_expectation_count].output_data_size = output_data_size;
  set_expectation_count++;
}

static void check_kind(const expectation *const current_expectation, int kind)
{
  char *message[sizeof(report_kind_error) + 10];

  if (current_expectation->kind == kind)
    return;

  sprintf((char*)&message[0], report_kind_error, get_expectation_count);
  FAIL((char*)&message[0]);
}

static void check_data(
    const expectation *const current_expectation,
    const uint8_t *const data
)
{
  char *message[sizeof(report_data_error) + 10];

  bool fail = false;
  for (uint16_t i = 0; i < current_expectation->output_data_size; i++)
  {
    if (current_expectation->output_data[i] != data[i])
    {
      fail = true;
      break;
    }
  }

  if (!fail)
    return;

  sprintf((char*)&message[0], report_data_error, get_expectation_count);
  FAIL((char*)&message[0]);
}


// Implementations -----------------------------------------------------------

void mock_flash_driver_io_create(const uint8_t max_expectations)
{
  if (expectations != NULL)
    free(expectations);
  expectations = calloc(max_expectations, sizeof(expectation));
  max_expectation_count = max_expectations;
}

void mock_flash_driver_io_destroy(void)
{
  if (expectations)
    free(expectations);
  expectations = NULL;

  set_expectation_count = 0;
  get_expectation_count = 0;
  max_expectation_count = 0;
}

void mock_flash_driver_io_expect_write(
  const uint8_t *const output_data,
  const uint16_t output_data_size
)
{
  fail_when_no_room_for_expectations();
  record_expectation(
    IO_WRITE,
    NULL,
    output_data,
    0,
    output_data_size
  );
}

void mock_flash_driver_io_expect_write_read(
  const uint8_t *const output_data,
  const uint8_t *const input_data,
  const uint16_t output_data_size,
  const uint16_t input_data_size
)
{
  fail_when_no_room_for_expectations();
  record_expectation(
    IO_WRITE_READ,
    input_data,
    output_data,
    input_data_size,
    output_data_size
  );
}

void mock_flash_driver_io_expect_select(bool select)
{
  fail_when_no_room_for_expectations();
  record_expectation(
    select ? IO_SELECT : IO_DISELECT,
    NULL,
    NULL,
    0,
    0
  );
}

void mock_flash_driver_io_verify_complete(void)
{
    char *message[sizeof(report_verify_error) + 10];

    if (set_expectation_count == get_expectation_count)
        return;
    
    sprintf(
        (char*)&message[0],
        report_verify_error,
        set_expectation_count,
        get_expectation_count
    );
    FAIL((char*)&message[0]);
}

// Implementations from flash_driver_io --------------------------------------------

flash_driver_status flash_driver_io_write_read(
  uint8_t *const output_data,
  uint8_t *const input_data,
  const uint16_t output_data_size
)
{
  expectation current_expectation = expectations[get_expectation_count];

  fail_when_no_init();
  check_kind(&current_expectation, IO_WRITE_READ);
  check_data(&current_expectation, output_data);

  memcpy(input_data, current_expectation.input_data, output_data_size);

  get_expectation_count++;

  return FLASH_DRIVER_OK;
}

flash_driver_status flash_driver_io_write(
  uint8_t *const output_data,
  const uint16_t output_data_size
)
{
  expectation current_expectation = expectations[get_expectation_count];

  fail_when_no_init();
  check_kind(&current_expectation, IO_WRITE);
  check_data(&current_expectation, output_data);

  get_expectation_count++;

  return FLASH_DRIVER_OK;
}

void flash_driver_select(bool select)
{
  expectation current_expectation = expectations[get_expectation_count];

  fail_when_no_init();
  check_kind(&current_expectation, select ? IO_SELECT : IO_DISELECT);

  get_expectation_count++;
}
