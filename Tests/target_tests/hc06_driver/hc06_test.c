#include "main.h"
#include "unity_fixture.h"
#include "hc06_driver.h"
#include "builder_hc06_driver.h"

// Global variables ----------------------------------------------------------

extern UART_HandleTypeDef huart2;

// Static functions ----------------------------------------------------------

static void check_data(
  const uint8_t *const expected,
  const uint8_t *const actual,
  const uint16_t data_size
)
{
  for (uint16_t i = 0; i < data_size; i++)
    TEST_ASSERT_BYTES_EQUAL(expected[i], actual[i]);
}

// Tests ---------------------------------------------------------------------

TEST_GROUP(hc06_driver);

TEST_SETUP(hc06_driver)
{
  builder_hc06_driver_build(&huart2);
}

TEST_TEAR_DOWN(hc06_driver)
{
  hc06_set_baudrate(HC06_9600);
  hc06_destroy();
}

TEST(hc06_driver, check_link_success)
{
  hc06_status status = hc06_check_link();

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, set_baud_rate_9600)
{
  hc06_status status = hc06_set_baudrate(HC06_9600);
  hc06_baudrate baudrate = hc_06_determine_baudrate();

  TEST_ASSERT_EQUAL(HC06_OK, status);
  TEST_ASSERT_EQUAL(HC06_9600, baudrate);
}

TEST(hc06_driver, set_baud_rate_1200)
{
  hc06_status status = hc06_set_baudrate(HC06_1200);
  hc06_baudrate baudrate = hc_06_determine_baudrate();

  TEST_ASSERT_EQUAL(HC06_OK, status);
  TEST_ASSERT_EQUAL(HC06_1200, baudrate);
}

TEST(hc06_driver, set_baud_rate_115200)
{
  hc06_status status = hc06_set_baudrate(HC06_115200);
  hc06_baudrate baudrate = hc_06_determine_baudrate();

  TEST_ASSERT_EQUAL(HC06_OK, status);
  TEST_ASSERT_EQUAL(HC06_115200, baudrate);
}
