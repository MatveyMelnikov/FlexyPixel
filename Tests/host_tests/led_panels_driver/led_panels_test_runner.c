#include "unity_fixture.h"

TEST_GROUP_RUNNER(led_panels_driver)
{
  RUN_TEST_CASE(led_panels_driver, create_buffer_and_its_clear);
  RUN_TEST_CASE(led_panels_driver, create_buffer_and_get_panels_success);
  RUN_TEST_CASE(led_panels_driver, create_buffer_and_get_panels_fail);
  RUN_TEST_CASE(led_panels_driver, send_one_panel_8_data);
  RUN_TEST_CASE(led_panels_driver, send_one_panel_16_data);
  RUN_TEST_CASE(led_panels_driver, send_two_panels_8_and_8_data);
  RUN_TEST_CASE(led_panels_driver, send_two_panels_8_and_16_data);
}