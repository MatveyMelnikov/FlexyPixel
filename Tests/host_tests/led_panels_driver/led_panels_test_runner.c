#include "unity_fixture.h"

TEST_GROUP_RUNNER(led_panels_driver)
{
  RUN_TEST_CASE(led_panels_driver, create_buffer_and_its_clear);
  RUN_TEST_CASE(led_panels_driver, create_buffer_and_get_panels_success);
  RUN_TEST_CASE(led_panels_driver, create_buffer_and_get_panels_fail);
  RUN_TEST_CASE(led_panels_driver, set_pixels_in_panel_8);
  RUN_TEST_CASE(led_panels_driver, set_pixels_in_panel_16);
  RUN_TEST_CASE(led_panels_driver, set_pixels_in_panels_8_and_16);
  // RUN_TEST_CASE(led_panels_driver, send_two_panels_8_and_16_data);
}