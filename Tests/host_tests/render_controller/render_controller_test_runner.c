#include "unity_fixture.h"

TEST_GROUP_RUNNER(render_controller)
{
  RUN_TEST_CASE(render_controller, render_controller_start);
  RUN_TEST_CASE(render_controller, data_cmd_unconfigured);
  RUN_TEST_CASE(render_controller, mode_cmd_success);
  RUN_TEST_CASE(render_controller, multiple_mode_cmd_success);
  RUN_TEST_CASE(render_controller, mode_cmd_and_data_error);
  RUN_TEST_CASE(render_controller, conf_cmd_success);
  RUN_TEST_CASE(render_controller, multiple_conf_cmd_success);
  RUN_TEST_CASE(render_controller, conf_cmd_and_data_error);
  RUN_TEST_CASE(render_controller, conf_plus_mode_cmd_and_data_success);
  RUN_TEST_CASE(render_controller, conf_plus_mode_cmd_and_data_panel_pos_error);
  RUN_TEST_CASE(render_controller, conf_plus_mode_cmd_and_data_panel_pos_error);
}