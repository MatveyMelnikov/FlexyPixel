#include "unity_fixture.h"

TEST_GROUP_RUNNER(hc06_driver)
{
  RUN_TEST_CASE(hc06_driver, check_link_success);
  RUN_TEST_CASE(hc06_driver, set_baud_rate_9600);
  RUN_TEST_CASE(hc06_driver, set_baud_rate_1200);
  RUN_TEST_CASE(hc06_driver, set_baud_rate_115200);
}