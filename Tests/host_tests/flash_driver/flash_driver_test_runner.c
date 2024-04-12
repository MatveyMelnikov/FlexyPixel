#include "unity_fixture.h"

TEST_GROUP_RUNNER(flash_driver)
{
  RUN_TEST_CASE(flash_driver, check_link_success);
  RUN_TEST_CASE(flash_driver, check_link_error);
  RUN_TEST_CASE(flash_driver, check_link_busy_error);
  RUN_TEST_CASE(flash_driver, write_success);
  RUN_TEST_CASE(flash_driver, read_success);
}