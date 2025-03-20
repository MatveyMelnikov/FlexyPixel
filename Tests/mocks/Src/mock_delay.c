#include "mock_delay.h"
#include "mock.h"
#include "unity_fixture.h"
#include <string.h>
#include <stdio.h>

// Defines -------------------------------------------------------------------

enum {
  OUTPUT_CHARS_ADDITIVE = 50U
};

// Static variables ----------------------------------------------------------

static const char *name = "Delay";
static mock delay_mock;

// Static prototypes ---------------------------------------------------------

// Implementations -----------------------------------------------------------

void mock_delay_create(const uint8_t max_expectations)
{
  delay_mock = mock_create(name, max_expectations);
}

void mock_delay_destroy()
{
  mock_destroy(&delay_mock);
}

void mock_delay_expect_delay(const uint32_t *const timout)
{
  mock_record_expectation(
    &delay_mock,
    SET,
    (uint8_t*)timout,
    sizeof(uint32_t)
  );
}

void mock_delay_verify_complete()
{
  mock_verify_complete(&delay_mock);
}

void mock_delay_delay(uint32_t timeout)
{
  expectation current_expectation = mock_get_expectation(&delay_mock);

  mock_check_kind(&delay_mock, &current_expectation, SET);
  mock_check_data(
    &delay_mock,
    &current_expectation,
    (uint8_t*)&timeout,
    sizeof(uint32_t)
  );
}
