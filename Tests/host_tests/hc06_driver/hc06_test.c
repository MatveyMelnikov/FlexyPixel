#include "unity_fixture.h"
#include "hc06_driver.h"
#include "mock_hc06_io.h"
#include <string.h>

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
  mock_hc06_io_create(10);
  mock_hc06_io_expect_baudrate_change();
  hc06_create();
}

TEST_TEAR_DOWN(hc06_driver)
{
  mock_hc06_io_expect_baudrate_change();
  hc06_destroy();
  mock_hc06_io_verify_complete();
  mock_hc06_io_destroy();
}


// hc06 will enter to the AT mode if it needn’t pair
// (Guangzhou HC IT HC-06 product datasheet pg. 16).
TEST(hc06_driver, check_link_success)
{
  char* output_data = "AT\r\n";
  char* input_data = "OK\r\n";

  mock_hc06_io_expect_write((uint8_t*)output_data, strlen(output_data));
  mock_hc06_io_expect_read_then_return(
    (uint8_t*)input_data,
    strlen(input_data)
  );

  hc06_status status = hc06_check_link();

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, set_baud_rate_1200)
{
  char* output_data = "AT+UART=1200,0,0\r\n";
  char* input_data = "OK1200\r\n";

  mock_hc06_io_expect_write(
    (uint8_t*)output_data,
    strlen(output_data)
  );
  mock_hc06_io_expect_read_then_return(
    (uint8_t*)input_data,
    strlen(input_data)
  );
  mock_hc06_io_expect_baudrate_change();

  hc06_status status = hc06_set_baudrate(HC06_1200);

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, set_baud_rate_9600)
{
  char* output_data = "AT+UART=9600,0,0\r\n";
  char* input_data = "OK9600\r\n";

  mock_hc06_io_expect_write(
    (uint8_t*)output_data,
    strlen(output_data)
  );
  mock_hc06_io_expect_read_then_return(
    (uint8_t*)input_data,
    strlen(input_data)
  );
  mock_hc06_io_expect_baudrate_change();

  hc06_status status = hc06_set_baudrate(HC06_9600);

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, set_baud_rate_460800)
{
  char* output_data = "AT+UART=460800,0,0\r\n";
  char* input_data = "OK460800\r\n";

  mock_hc06_io_expect_write(
    (uint8_t*)output_data,
    strlen(output_data)
  );
  mock_hc06_io_expect_read_then_return(
    (uint8_t*)input_data,
    strlen(input_data)
  );
  mock_hc06_io_expect_baudrate_change();

  hc06_status status = hc06_set_baudrate(HC06_460800);

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, set_name_anything_success)
{
  char* name = "anything";
  char* output_data = "AT+NAME=anything\r\n";
  char* input_data = "OKname\r\n";

  mock_hc06_io_expect_write(
    (uint8_t*)output_data,
    strlen(output_data)
  );
  mock_hc06_io_expect_read_then_return(
    (uint8_t*)input_data,
    strlen(input_data)
  );

  hc06_status status = hc06_set_name(name);

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, set_name_new_name_success)
{
  char* name = "new_name";
  char* output_data = "AT+NAME=new_name\r\n";
  char* input_data = "OKname\r\n";

  mock_hc06_io_expect_write(
    (uint8_t*)output_data,
    strlen(output_data)
  );
  mock_hc06_io_expect_read_then_return(
    (uint8_t*)input_data,
    strlen(input_data)
  );

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
  char* output_data = "AT+PSWD=\"1234\"\r\n";
  char* input_data = "OKsetpin\r\n";

  mock_hc06_io_expect_write(
    (uint8_t*)output_data,
    strlen(output_data)
  );
  mock_hc06_io_expect_read_then_return(
    (uint8_t*)input_data,
    strlen(input_data)
  );

  hc06_status status = hc06_set_pin(1234);

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, set_12345_pin_error)
{
  hc06_status status = hc06_set_pin(12345);

  TEST_ASSERT_EQUAL(HC06_ERROR, status);
}

TEST(hc06_driver, set_0000_pin_success)
{
  char* output_data = "AT+PSWD=\"0000\"\r\n";
  char* input_data = "OKsetpin\r\n";

  mock_hc06_io_expect_write(
    (uint8_t*)output_data,
    strlen(output_data)
  );
  mock_hc06_io_expect_read_then_return(
    (uint8_t*)input_data,
    strlen(input_data)
  );

  hc06_status status = hc06_set_pin(0000);

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, write)
{
  uint8_t output_data[] = { 0xff, 0xcc, 0x55, 0xaa };

  mock_hc06_io_expect_write(
    output_data,
    sizeof(output_data)
  );

  hc06_status status = hc06_write(output_data, sizeof(output_data));

  TEST_ASSERT_EQUAL(HC06_OK, status);
}

TEST(hc06_driver, read)
{
  uint8_t input_data[] = { 0xff, 0xcc, 0x55, 0xaa };

  mock_hc06_io_expect_read_external_then_return(
    input_data,
    sizeof(input_data)
  );

  hc06_status status = hc06_read(input_data, sizeof(input_data));

  TEST_ASSERT_EQUAL(HC06_OK, status);
}
