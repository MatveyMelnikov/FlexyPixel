#include "unity_fixture.h"
#include "hc06_driver.h"
#include "mock_uart.h"
#include "mock_delay.h"
#include <string.h>

// Static variables ----------------------------------------------------------

static char *ok_response = "OK";
static uint32_t init_speed = 1200U;
static uint32_t normal_speed = 9600U;
static uint32_t highest_speed = 115200U;
static uint32_t at_delay = 100U;
static char *test_cmd = "AT";

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

static hc06_status hc06_io_blocking_read(
  uint8_t *const data,
  const uint16_t data_size
)
{
  return (hc06_status)mock_uart_receive(data, data_size);
}

static hc06_status hc06_io_write(
  const uint8_t *const data,
  const uint16_t data_size
)
{
  return (hc06_status)mock_uart_transmit((uint8_t *const)data, data_size);
}

static hc06_status hc06_io_non_blocking_read(
  uint8_t *const data,
  const uint16_t data_size
)
{
  hc06_receive_complete();
  return (hc06_status)mock_uart_receive(data, data_size);
}

static hc06_status hc06_io_set_baudrate(const uint32_t baudrate)
{
  return (hc06_status)mock_uart_set_baudrate(baudrate);
}

static void expect_transmit_receive_cmd(
  const char *const cmd,
  const char *const response
)
{
  mock_uart_expect_transmit((uint8_t*)cmd, strlen(cmd));
  mock_uart_expect_receive((uint8_t*)response, strlen(response));
  mock_delay_expect_delay(&at_delay);
}

static void expect_check_link()
{
  expect_transmit_receive_cmd(test_cmd, ok_response);
  expect_transmit_receive_cmd(test_cmd, ok_response);
}

static void expect_module_init()
{
  char *set_baudrate_cmd = "AT+BAUD4";

  // Determine baudrate
  mock_uart_expect_set_baudrate(&init_speed);
  expect_check_link();

  // Set baudrate
  expect_transmit_receive_cmd(set_baudrate_cmd, ok_response);
  mock_uart_expect_set_baudrate(&normal_speed);
}

// Tests ---------------------------------------------------------------------

TEST_GROUP(hc06_driver);

TEST_SETUP(hc06_driver)
{
  mock_uart_create(20U);
  mock_delay_create(10U);

  expect_module_init();
  hc06_create(
    (hc06_io) {
      .write = hc06_io_write,
      .blocking_read = hc06_io_blocking_read,
      .non_blocking_read = hc06_io_non_blocking_read,
      .delay = mock_delay_delay,
      .set_controller_baudrate = hc06_io_set_baudrate
    }
  );
}

TEST_TEAR_DOWN(hc06_driver)
{
  mock_uart_expect_set_baudrate(&normal_speed);
  hc06_destroy();
  mock_uart_verify_complete();
  mock_uart_destroy();
}

TEST(hc06_driver, driver_create_succes)
{
  // Expect in test setup
}

// hc06 will enter to the AT mode if it needn’t pair
// (Guangzhou HC IT HC-06 product datasheet pg. 16).
TEST(hc06_driver, check_link_success)
{
  char* output_data = "AT";
  char* input_data = "OK";

  // expect_transmit_receive_cmd(output_data, input_data);
  expect_check_link();

  hc06_status status = hc06_check_link();

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, set_baud_rate_1200_success)
{
  char* output_data = "AT+BAUD1";
  char* input_data = "OK";

  // Determine baudrate
  mock_uart_expect_set_baudrate(&init_speed);
  expect_check_link();

  // Set baudrate
  expect_transmit_receive_cmd(output_data, input_data);
  mock_uart_expect_set_baudrate(&init_speed);

  hc06_status status = hc06_set_baudrate(HC06_1200);

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, set_baud_rate_9600_success)
{
  char* output_data = "AT+BAUD4";
  char* input_data = "OK";

  // Determine baudrate
  mock_uart_expect_set_baudrate(&init_speed);
  expect_check_link();

  // Set baudrate
  expect_transmit_receive_cmd(output_data, input_data);
  mock_uart_expect_set_baudrate(&normal_speed);

  hc06_status status = hc06_set_baudrate(HC06_9600);

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, set_baud_rate_115200_success)
{
  char* output_data = "AT+BAUD8";
  char* input_data = "OK";

  // Determine baudrate
  mock_uart_expect_set_baudrate(&init_speed);
  expect_check_link();

  // Set baudrate
  expect_transmit_receive_cmd(output_data, input_data);
  mock_uart_expect_set_baudrate(&highest_speed);

  hc06_status status = hc06_set_baudrate(HC06_115200);

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, set_name_anything_success)
{
  char* name = "anything";
  char* output_data = "AT+NAMEanything";
  char* input_data = "OK";
  
  expect_transmit_receive_cmd(output_data, input_data);
  hc06_status status = hc06_set_name(name);

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, set_name_long_name_fail)
{
  char* name = "super_long_name_aaaaa";

  hc06_status status = hc06_set_name(name);

  TEST_ASSERT_EQUAL(HC06_ERROR, status);
}

TEST(hc06_driver, set_1234_pin_success)
{
  char* output_data = "AT+PIN1234";
  char* input_data = "OK";

  expect_transmit_receive_cmd(output_data, input_data);
  hc06_status status = hc06_set_pin(1234);

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, set_12345_pin_fail)
{
  hc06_status status = hc06_set_pin(12345);

  TEST_ASSERT_EQUAL(HC06_ERROR, status);
}

TEST(hc06_driver, set_0000_pin_success)
{
  char* output_data = "AT+PIN0000";
  char* input_data = "OK";

  expect_transmit_receive_cmd(output_data, input_data);
  hc06_status status = hc06_set_pin(0000);

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, write_success)
{
  uint8_t output_data[] = { 0xff, 0xcc, 0x55, 0xaa };

  mock_uart_expect_transmit(output_data, sizeof(output_data));
  hc06_status status = hc06_write(output_data, sizeof(output_data));

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, read_success)
{
  uint8_t input_data[] = { 0xff, 0xcc, 0x55, 0xaa };

  mock_uart_expect_receive(input_data, sizeof(input_data));
  hc06_status status = hc06_read(input_data, sizeof(input_data));

  TEST_ASSERT_EQUAL(HC06_OK, status);
}
