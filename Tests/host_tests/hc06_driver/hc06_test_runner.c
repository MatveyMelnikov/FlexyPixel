#include "unity_fixture.h"

TEST_GROUP_RUNNER(hc06_driver)
{
  RUN_TEST_CASE(hc06_driver, check_link_success);
  RUN_TEST_CASE(hc06_driver, set_baud_rate_1200);
  RUN_TEST_CASE(hc06_driver, set_baud_rate_9600);
  RUN_TEST_CASE(hc06_driver, set_baud_rate_460800);
  RUN_TEST_CASE(hc06_driver, set_name_anything_success);
  RUN_TEST_CASE(hc06_driver, set_name_new_name_success);
  RUN_TEST_CASE(hc06_driver, set_name_long_name_fail);
  RUN_TEST_CASE(hc06_driver, set_1234_pin_success);
  RUN_TEST_CASE(hc06_driver, set_12345_pin_error);
  RUN_TEST_CASE(hc06_driver, set_0000_pin_success);
  RUN_TEST_CASE(hc06_driver, write);
  RUN_TEST_CASE(hc06_driver, read);
}