/*
Due to the use of bit banding to speed up interrupt processing,
only pixel data is tested here. The sending itself is tested in target tests
*/

#include "unity_fixture.h"
#include "led_panels_driver.h"
#include <string.h>

// Defines -------------------------------------------------------------------

#define PIXEL_SIZE 1.5f

// Static variables ----------------------------------------------------------

static led_panels_buffer *buffer = NULL;
const static led_panels_color pixel = (led_panels_color) {
  .red = 9U,
  .green = 6U,
  .blue = 3U
};

// Static functions ----------------------------------------------------------

static void set_data_pixel(
  uint8_t *data,
  uint8_t size_x,
  uint8_t pos_x,
  uint8_t pos_y
)
{
  uint16_t x = (pos_y % 2 == 0 ? (size_x - 1 - pos_x) : pos_x);
  uint16_t pixel_num = x + pos_y * size_x;
  uint16_t byte_num = ((pixel_num / 2) * 3) + (pixel_num % 2);

  bool is_odd = pixel_num % 2 == 1;
  if (is_odd)
  {
    data[byte_num] &= 0xf0;
    data[byte_num] |= pixel.red;
    data[byte_num + 1] = (pixel.green << 4) | pixel.blue;
  }
  else
  {
    data[byte_num] = (pixel.red << 4) | pixel.green;
    data[byte_num + 1] &= 0x0f;
    data[byte_num + 1] |= pixel.blue << 4;
  }
}

// Tests ---------------------------------------------------------------------

TEST_GROUP(led_panels_driver);

TEST_SETUP(led_panels_driver)
{
  led_panels_size panels_types[] = {
    LED_PANELS_SIZE_64
  };

  buffer = led_panels_create(1, panels_types);
}

TEST_TEAR_DOWN(led_panels_driver)
{
  led_panels_destroy(buffer);
}

TEST(led_panels_driver, create_buffer_and_its_clear)
{
  led_panels_color pixel = { 0 };

  led_panels_status status = LED_PANELS_OK;
  for (uint8_t y = 0; y < 0; y++)
  {
    for (uint8_t x = 0; x < 0; x++)
    {
      status = led_panels_get_pixel(buffer, x, y, 0, &pixel);
      TEST_ASSERT_EQUAL(LED_PANELS_OK, status);
      TEST_ASSERT_EQUAL(0, pixel.red);
      TEST_ASSERT_EQUAL(0, pixel.green);
      TEST_ASSERT_EQUAL(0, pixel.blue);
    }
  }
}

TEST(led_panels_driver, create_buffer_and_get_panels_success)
{
  led_panels_size sizes[] = {
    LED_PANELS_SIZE_64,
    LED_PANELS_SIZE_256,
    LED_PANELS_SIZE_64
  };
  led_panels_buffer *local_buffer = led_panels_create(3, sizes);
  uint16_t panel_size = 0;
  led_panels_color pixel = { 0 };

  led_panels_status status = led_panels_get_pixel(
    local_buffer, 0, 7, 7, &pixel
  );
  status |= led_panels_get_pixel(local_buffer, 1, 15, 15, &pixel);
  status |= led_panels_get_pixel(local_buffer, 2, 7, 7, &pixel);

  led_panels_destroy(local_buffer);

  TEST_ASSERT_EQUAL(LED_PANELS_OK, status);
}

TEST(led_panels_driver, create_buffer_and_get_panels_fail)
{
  led_panels_size sizes[] = {
    LED_PANELS_SIZE_64,
    LED_PANELS_SIZE_256,
    LED_PANELS_SIZE_64
  };
  led_panels_buffer *buffer = led_panels_create(3, sizes);
  led_panels_color pixel = { 0 };

  led_panels_status status = led_panels_get_pixel(buffer, 0, 8, 7, &pixel);
  TEST_ASSERT_EQUAL(LED_PANELS_BOUNDS, status);
  status = led_panels_get_pixel(buffer, 0, 15, 20, &pixel);
  TEST_ASSERT_EQUAL(LED_PANELS_BOUNDS, status);
  status = led_panels_get_pixel(buffer, 4, 4, 1, &pixel);
  TEST_ASSERT_EQUAL(LED_PANELS_BOUNDS, status);

  led_panels_destroy(buffer);
}

TEST(led_panels_driver, set_pixels_in_panel_8)
{
  uint16_t data_size = 64 * PIXEL_SIZE;
  uint8_t data[data_size];
  memset(data, 0, data_size);

  set_data_pixel(data, 8, 0, 0);
  set_data_pixel(data, 8, 7, 0);
  set_data_pixel(data, 8, 0, 7);
  set_data_pixel(data, 8, 7, 7);

  led_panels_status status = led_panels_set_pixel(buffer, 0, 0, 0, pixel);
  status |= led_panels_set_pixel(buffer, 0, 7, 0, pixel);
  status |= led_panels_set_pixel(buffer, 0, 0, 7, pixel);
  status |= led_panels_set_pixel(buffer, 0, 7, 7, pixel);

  TEST_ASSERT_EQUAL(LED_PANELS_OK, status);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(data, buffer->pixel_data, data_size);
}

TEST(led_panels_driver, set_pixels_in_panel_16)
{
  led_panels_size sizes[] = { LED_PANELS_SIZE_256 };
  led_panels_buffer *local_buffer = led_panels_create(1, sizes);

  uint16_t data_size = 256 * PIXEL_SIZE;
  uint8_t data[data_size];
  memset(data, 0, data_size);

  set_data_pixel(data, 16, 0, 0);
  set_data_pixel(data, 16, 15, 0);
  set_data_pixel(data, 16, 0, 15);
  set_data_pixel(data, 16, 15, 15);

  led_panels_status status = led_panels_set_pixel(
    local_buffer, 0, 0, 0, pixel
  );
  status |= led_panels_set_pixel(local_buffer, 0, 15, 0, pixel);
  status |= led_panels_set_pixel(local_buffer, 0, 0, 15, pixel);
  status |= led_panels_set_pixel(local_buffer, 0, 15, 15, pixel);

  TEST_ASSERT_EQUAL(LED_PANELS_OK, status);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(data, local_buffer->pixel_data, data_size);

  led_panels_destroy(local_buffer);
}

TEST(led_panels_driver, set_pixels_in_panels_8_and_16)
{
  led_panels_size sizes[] = { LED_PANELS_SIZE_64, LED_PANELS_SIZE_256 };
  led_panels_buffer *local_buffer = led_panels_create(2, sizes);

  uint16_t data_size = 320 * PIXEL_SIZE;
  uint16_t panel_16_offset = 64 * PIXEL_SIZE;
  uint8_t data[data_size];
  memset(data, 0, data_size);

  set_data_pixel(data, 8, 0, 0);
  set_data_pixel(data, 8, 7, 0);
  set_data_pixel(data, 8, 0, 7);
  set_data_pixel(data, 8, 7, 7);
  set_data_pixel(data + panel_16_offset, 16, 0, 0);
  set_data_pixel(data + panel_16_offset, 16, 15, 0);
  set_data_pixel(data + panel_16_offset, 16, 0, 15);
  set_data_pixel(data + panel_16_offset, 16, 15, 15);

  led_panels_status status = led_panels_set_pixel(
    local_buffer, 0, 0, 0, pixel
  );
  status |= led_panels_set_pixel(local_buffer, 0, 7, 0, pixel);
  status |= led_panels_set_pixel(local_buffer, 0, 0, 7, pixel);
  status |= led_panels_set_pixel(local_buffer, 0, 7, 7, pixel);
  status |= led_panels_set_pixel(local_buffer, 1, 0, 0, pixel);
  status |= led_panels_set_pixel(local_buffer, 1, 15, 0, pixel);
  status |= led_panels_set_pixel(local_buffer, 1, 0, 15, pixel);
  status |= led_panels_set_pixel(local_buffer, 1, 15, 15, pixel);

  TEST_ASSERT_EQUAL(LED_PANELS_OK, status);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(data, local_buffer->pixel_data, data_size);

  led_panels_destroy(local_buffer);
}
