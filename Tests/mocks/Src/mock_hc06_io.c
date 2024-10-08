#include "mock_hc06_io.h"
#include "hc06_driver.h"
#include "unity_fixture.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
  int kind;
  const uint8_t * data;
  uint8_t data_size
} expectation;

enum
{
  IO_READ,
  IO_EXTERNAL_READ,
  IO_WRITE,
  IO_BAUDRATE,
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
  const uint8_t *const data,
  const uint8_t data_size
)
{
  expectations[set_expectation_count].kind = kind;
  expectations[set_expectation_count].data = data;
  expectations[set_expectation_count].data_size = data_size;
  set_expectation_count++;
}

static void check_kind(const expectation *const current_expectation, int kind)
{
  char *message[sizeof(report_kind_error) + 10];

  if (current_expectation->kind == kind)
    return;

  sprintf(message, report_kind_error, get_expectation_count);
  FAIL(message);
}

static void check_data(
  const expectation *const current_expectation,
  const uint8_t *const data
)
{
  char *message[sizeof(report_data_error) + 10];

  bool fail = false;
  for (uint8_t i = 0; i < current_expectation->data_size; i++)
  {
    if (current_expectation->data[i] != data[i])
    {
      fail = true;
      break;
    }
  }

  if (!fail)
    return;

  sprintf(message, report_data_error, get_expectation_count);
  FAIL(message);
}


// Implementations -----------------------------------------------------------

void mock_hc06_io_create(const uint8_t max_expectations)
{
  if (expectations != NULL)
    free(expectations);
  expectations = calloc(max_expectations, sizeof(expectation));
  max_expectation_count = max_expectations;
}

void mock_hc06_io_destroy(void)
{
  if (expectations)
    free(expectations);
  expectations = NULL;

  set_expectation_count = 0;
  get_expectation_count = 0;
  max_expectation_count = 0;
}

void mock_hc06_io_expect_write(
  const uint8_t *const data,
  const uint16_t data_size
)
{
  fail_when_no_room_for_expectations();
  record_expectation(IO_WRITE, data, data_size);
}

void mock_hc06_io_expect_read_then_return(
  const uint8_t *const data,
  const uint16_t data_size
)
{
  fail_when_no_room_for_expectations();
  record_expectation(IO_READ, data, data_size);
}

void mock_hc06_io_expect_read_external_then_return(
  const uint8_t *const data,
  const uint16_t data_size
)
{
  fail_when_no_room_for_expectations();
  record_expectation(IO_EXTERNAL_READ, data, data_size);
  hc06_receive_complete();
}

void mock_hc06_io_expect_baudrate_change(void)
{
  fail_when_no_room_for_expectations();
  record_expectation(IO_BAUDRATE, NULL, 0);
}

void mock_hc06_io_verify_complete(void)
{
  char *message[sizeof(report_verify_error) + 10];

  if (set_expectation_count == get_expectation_count)
    return;
    
  sprintf(
    message,
    report_verify_error,
    set_expectation_count,
    get_expectation_count
  );
  FAIL(message);
}

// Implementations from hc06_io --------------------------------------------

hc06_status hc06_io_read(
  uint8_t *const data,
  const uint16_t data_size
)
{
  expectation current_expectation = expectations[get_expectation_count];

  fail_when_no_init();
  check_kind(&current_expectation, IO_READ);

  memcpy(data, current_expectation.data, data_size);
    
  get_expectation_count++;
  return HC06_OK;
}

hc06_status hc06_io_write(
  const uint8_t *const data,
  const uint16_t data_size
)
{
  expectation current_expectation = expectations[get_expectation_count];

  fail_when_no_init();
  check_kind(&current_expectation, IO_WRITE);
  check_data(&current_expectation, data);

  get_expectation_count++;

  return HC06_OK;
}

hc06_status hc06_io_read_external_data(
  uint8_t *const data,
  const uint16_t data_size
)
{
  expectation current_expectation = expectations[get_expectation_count];

  fail_when_no_init();
  check_kind(&current_expectation, IO_EXTERNAL_READ);

  memcpy(data, current_expectation.data, data_size);
    
  get_expectation_count++;
  return HC06_OK;
}

hc06_status hc06_io_set_baudrate(
  const uint32_t baudrate
)
{
  expectation current_expectation = expectations[get_expectation_count];

  fail_when_no_init();
  check_kind(&current_expectation, IO_BAUDRATE);
    
  get_expectation_count++;
  return HC06_OK;
}
