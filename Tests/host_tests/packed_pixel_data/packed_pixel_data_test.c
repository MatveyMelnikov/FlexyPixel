#include "unity_fixture.h"
#include "packed_pixel_data.h"
#include <string.h>

// Static variables ----------------------------------------------------------

static char *ok_response = "OK";
static uint32_t init_speed = 1200U;
static uint32_t normal_speed = 9600U;
static uint32_t highest_speed = 115200U;
static uint32_t at_delay = 100U;
static char *test_cmd = "AT";

// Static functions ----------------------------------------------------------

// Tests ---------------------------------------------------------------------

TEST_GROUP(packed_pixel_data);

TEST_SETUP(packed_pixel_data)
{
}

TEST_TEAR_DOWN(packed_pixel_data)
{
}

TEST(packed_pixel_data, pack_even_line_is_ok)
{
  packed_pixel_data_color input[864] = { 0 };
  uint8_t output[864] = { 0 };
  uint8_t expected_line[12] = { 
    0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x19U, 0x0U, 0x90U, 0x09U, 0U
  };

  for (uint8_t i = 0; i < 3U; i++)
  {
    // First string (index = 0)
    input[i] = (packed_pixel_data_color) {
      .red = 9U,
      .green = 0U,
      .blue = 0U
    };
  }
  for (uint8_t i = 0; i < 5U; i++)
  {
    // First string (index = 0)
    input[3U + i] = (packed_pixel_data_color) {
      .red = 1U,
      .green = 1U,
      .blue = 1U
    };
  }

  packed_pixel_data_pack(input, output, 1U);

  TEST_ASSERT_EQUAL_UINT8_ARRAY(
    (uint8_t*)&expected_line,
    output,
    sizeof(expected_line)
  );
}

TEST(packed_pixel_data, pack_odd_line_is_ok)
{
  packed_pixel_data_color input[864] = { 0 };
  uint8_t output[864] = { 0 };
  uint8_t expected_line[12] = { 
    0x90U, 0x09U, 0U, 0x90, 0x01, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U
  };

  for (uint8_t i = 0; i < 3U; i++)
  {
    // Second string (index = 1)
    input[8U + i] = (packed_pixel_data_color) {
      .red = 9U,
      .green = 0U,
      .blue = 0U
    };
  }
  for (uint8_t i = 0; i < 5U; i++)
  {
    // Second string (index = 1)
    input[8U + 3U + i] = (packed_pixel_data_color) {
      .red = 1U,
      .green = 1U,
      .blue = 1U
    };
  }

  packed_pixel_data_pack(input, output, 1U);

  TEST_ASSERT_EQUAL_UINT8_ARRAY(
    (uint8_t*)&expected_line,
    output + 12U,
    sizeof(expected_line)
  );
}

TEST(packed_pixel_data, unpack_even_line_is_ok)
{
  uint8_t input[864] = { 0 };
  uint8_t input_line[12] = { 
    0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x19U, 0x0U, 0x90U, 0x09U, 0U
  };
  memcpy(input, input_line, sizeof(input_line));

  packed_pixel_data_color output[864] = { 0 };
  packed_pixel_data_color expected_output[8] = { 0 };

  for (uint8_t i = 0; i < 3U; i++)
  {
    // First string (index = 0)
    expected_output[i] = (packed_pixel_data_color) {
      .red = 9U,
      .green = 0U,
      .blue = 0U
    };
  }
  for (uint8_t i = 0; i < 5U; i++)
  {
    // First string (index = 0)
    expected_output[3U + i] = (packed_pixel_data_color) {
      .red = 1U,
      .green = 1U,
      .blue = 1U
    };
  }

  packed_pixel_data_unpack(input, output, 1U);

  TEST_ASSERT_EQUAL_UINT8_ARRAY(
    (uint8_t*)&expected_output,
    output,
    sizeof(expected_output)
  );
}

TEST(packed_pixel_data, unpack_odd_line_is_ok)
{
  uint8_t input[864] = { 0 };
  uint8_t input_line[12] = { 
    0x90U, 0x09U, 0U, 0x90, 0x01, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U
  };
  memcpy(input + 12, input_line, sizeof(input_line));

  packed_pixel_data_color output[864] = { 0 };
  packed_pixel_data_color expected_output[8] = { 0 };

  for (uint8_t i = 0; i < 3U; i++)
  {
    // Second string (index = 1)
    expected_output[i] = (packed_pixel_data_color) {
      .red = 9U,
      .green = 0U,
      .blue = 0U
    };
  }
  for (uint8_t i = 0; i < 5U; i++)
  {
    // Second string (index = 1)
    expected_output[3U + i] = (packed_pixel_data_color) {
      .red = 1U,
      .green = 1U,
      .blue = 1U
    };
  }

  packed_pixel_data_unpack(input, output, 1U);

  TEST_ASSERT_EQUAL_UINT8_ARRAY(
    (uint8_t*)&expected_output,
    output + 8,
    sizeof(expected_output)
  );
}

TEST(packed_pixel_data, pack_vertical_line_is_ok)
{
  uint8_t input[96] = { 
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x09U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0x90U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x09U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0x90U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x09U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U
  };
  packed_pixel_data_color output[576] = { 0 };
  packed_pixel_data_color expected[64] = { 0 };

  for (uint8_t i = 0; i < 5U; i++)
  {
    // x = 2, y = [0, 5]
    expected[2U + (8U * i)] = (packed_pixel_data_color) {
      .red = 9U,
      .green = 0U,
      .blue = 0U
    };
  }

  packed_pixel_data_unpack(input, output, 1U);

  TEST_ASSERT_EQUAL_UINT8_ARRAY(
    expected,
    output,
    sizeof(expected)
  );
}

TEST(packed_pixel_data, unpack_vertical_line_is_ok)
{
  packed_pixel_data_color input[576] = { 0 };
  uint8_t output[864] = { 0 };
  uint8_t expected[96] = { 
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x09U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0x90U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x09U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0x90U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x09U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U
  };

  for (uint8_t i = 0; i < 5U; i++)
  {
    // x = 2, y = [0, 5]
    input[2U + (8U * i)] = (packed_pixel_data_color) {
      .red = 9U,
      .green = 0U,
      .blue = 0U
    };
  }

  packed_pixel_data_pack(input, output, 1U);

  TEST_ASSERT_EQUAL_UINT8_ARRAY(
    expected,
    output,
    sizeof(expected)
  );
}
