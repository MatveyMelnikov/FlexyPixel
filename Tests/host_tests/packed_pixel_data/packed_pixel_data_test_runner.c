#include "unity_fixture.h"

TEST_GROUP_RUNNER(packed_pixel_data)
{
  RUN_TEST_CASE(packed_pixel_data, pack_even_line_is_ok);
  RUN_TEST_CASE(packed_pixel_data, pack_odd_line_is_ok);
  RUN_TEST_CASE(packed_pixel_data, unpack_even_line_is_ok);
  RUN_TEST_CASE(packed_pixel_data, unpack_odd_line_is_ok);
  RUN_TEST_CASE(packed_pixel_data, pack_vertical_line_is_ok);
  RUN_TEST_CASE(packed_pixel_data, unpack_vertical_line_is_ok);
}