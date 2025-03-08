#ifndef MOCK_DELAY_H
#define MOCK_DELAY_H

#include <stdint.h>
#include <stdbool.h>

void mock_delay_create(const uint8_t max_expectations);
void mock_delay_destroy(void);
void mock_delay_expect_delay(const uint32_t *const timout);
void mock_delay_verify_complete(void);

void mock_delay_delay(uint32_t timeout);

#endif
