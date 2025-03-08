#ifndef MOCK_H
#define MOCK_H

#include "mock_defs.h"

mock mock_create(const char *const mock_name, const uint8_t max_expectations);
void mock_destroy(mock *const self);
void mock_record_expectation(
  mock *const self,
  const int kind,
  const uint8_t *const data,
  const uint8_t data_size
);
expectation mock_get_expectation(mock *const self);
void mock_check_kind(
  mock *const self,
  const expectation *const current_expectation,
  int kind
);
void mock_check_data(
  mock *const self,
  const expectation *const current_expectation,
  const uint8_t *const data,
  const uint8_t size
);
void mock_check_data_size(
  mock *const self,
  const expectation *const current_expectation,
  const uint8_t size
);
void mock_verify_complete(mock *const self);

#endif
