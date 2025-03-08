#include "mock.h"
#include "unity_fixture.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// Defines -------------------------------------------------------------------

enum {
  OUTPUT_CHARS_ADDITIVE = 30U
};

// Static variables ----------------------------------------------------------

static char *report_not_init = "%s mock not initialized";
static char *report_no_room_for_expectation = "No room for expectations in "
  "%s mock";
static char *report_data_error = "Data error in %s mock";
static char *report_verify_error = "Verify error in %s mock. Expected %u "
  "operations, but got %u";
static char *report_kind_error = "Error kind in %s mock. "
  "Num of expectation %u ";
static char *report_index_error = "Index error in %s mock. Index: %u";

// Static prototypes ---------------------------------------------------------

static void mock_fail_when_no_room_for_expectations(mock *const self);
static void mock_fail_when_no_init(mock *const self);
static void mock_fail_with_report_not_init(mock *const self);
static void mock_fail_with_report_no_room_for_expectation(mock *const self);
static void mock_fail_with_report_data_error(mock *const self);
static void mock_fail_with_report_verify_error(mock *const self);
static void mock_fail_with_report_kind_error(mock *const self);
static void mock_fail_with_report_index_error(
  mock *const self,
  const uint8_t index
);

// Implementations -----------------------------------------------------------

mock mock_create(const char *const mock_name, const uint8_t max_expectations)
{
  return (mock) {
    .current_mock_name = mock_name,
    .expectations = calloc(max_expectations, sizeof(expectation)),
    .set_expectation_count = 0,
    .get_expectation_count = 0,
    .max_expectation_count = max_expectations
  };
}

void mock_destroy(mock *const self)
{
  free(self->expectations);
  self->expectations = NULL;
  self->current_mock_name = NULL;
  self->max_expectation_count = 0;
}

void mock_record_expectation(
  mock *const self,
  const int kind,
  const uint8_t *const data,
  const uint8_t data_size
)
{
  mock_fail_when_no_room_for_expectations(self);

  uint8_t set_expectation_count = self->set_expectation_count;

  self->expectations[set_expectation_count].kind = kind;
  self->expectations[set_expectation_count].data = data;
  self->expectations[set_expectation_count].data_size = data_size;
  self->set_expectation_count++;
}

expectation mock_get_expectation(mock *const self)
{
  mock_fail_when_no_init(self);

  if (self->get_expectation_count > self->set_expectation_count)
    mock_fail_with_report_index_error(self, self->get_expectation_count);

  expectation current_exception = self->expectations[
    self->get_expectation_count++
  ];

  return current_exception;
}

void mock_check_kind(
  mock *const self,
  const expectation *const current_expectation,
  int kind
)
{
  if (current_expectation->kind == kind)
    return;

  mock_fail_with_report_kind_error(self);
}

void mock_check_data(
  mock *const self,
  const expectation *const current_expectation,
  const uint8_t *const data,
  const uint8_t size
)
{
  mock_check_data_size(self, current_expectation, size);

  if (memcmp(current_expectation->data, data, size) != 0)
    mock_fail_with_report_data_error(self);
}

void mock_check_data_size(
  mock *const self,
  const expectation *const current_expectation,
  const uint8_t size
)
{
  if (current_expectation->data_size != size)
    mock_fail_with_report_data_error(self);
}

void mock_verify_complete(mock *const self)
{
  if (self->set_expectation_count == self->get_expectation_count)
    return;

  mock_fail_with_report_verify_error(self);
}

static void mock_fail_when_no_room_for_expectations(mock *const self)
{
  mock_fail_when_no_init(self);
  if (self->set_expectation_count >= self->max_expectation_count)
    mock_fail_with_report_no_room_for_expectation(self);
}

static void mock_fail_when_no_init(mock *const self)
{
  if (self->expectations == NULL)
    mock_fail_with_report_not_init(self);
}

static void mock_fail_with_report_not_init(mock *const self)
{
  char *message[sizeof(report_not_init) + OUTPUT_CHARS_ADDITIVE];

  sprintf(
    (char*)message,
    report_not_init,
    self->current_mock_name
  );
  FAIL((char*)message);
}

static void mock_fail_with_report_no_room_for_expectation(mock *const self)
{
  char *message[
    sizeof(report_no_room_for_expectation) + OUTPUT_CHARS_ADDITIVE
  ];
  
  sprintf(
    (char*)message,
    report_no_room_for_expectation,
    self->current_mock_name
  );
  FAIL((char*)message);
}

static void mock_fail_with_report_data_error(mock *const self)
{
  char *message[sizeof(report_data_error) + OUTPUT_CHARS_ADDITIVE];

  sprintf(
    (char*)message,
    report_data_error,
    self->current_mock_name
  );
  FAIL((char*)message);
}

static void mock_fail_with_report_verify_error(mock *const self)
{
  char *message[sizeof(report_verify_error) + OUTPUT_CHARS_ADDITIVE];

  sprintf(
    (char*)message,
    report_verify_error,
    self->current_mock_name,
    self->set_expectation_count,
    self->get_expectation_count
  );
  FAIL((char*)message);
}

static void mock_fail_with_report_kind_error(mock *const self)
{
  char *message[sizeof(report_kind_error) + OUTPUT_CHARS_ADDITIVE];

  sprintf(
    (char*)message,
    report_kind_error,
    self->current_mock_name,
    self->get_expectation_count
  );
  FAIL((char*)message);
}

static void mock_fail_with_report_index_error(
  mock *const self,
  const uint8_t index
)
{
  char *message[sizeof(report_index_error) + OUTPUT_CHARS_ADDITIVE];

  sprintf(
    (char*)message,
    report_index_error,
    self->current_mock_name,
    index
  );
  FAIL((char*)message);
}
