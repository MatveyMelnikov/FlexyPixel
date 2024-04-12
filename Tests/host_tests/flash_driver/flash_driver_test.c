#include "unity_fixture.h"
#include "flash_driver.h"
#include "mock_flash_driver_io.h"
#include <string.h>
#include <memory.h>

// Defines -------------------------------------------------------------------

#define ADD_ADDR_IN_BYTES(arr, addr) \
  arr[0] = (uint8_t)((addr) >> 8); \
  arr[1] = (uint8_t)(addr)

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

TEST_GROUP(flash_driver);

TEST_SETUP(flash_driver)
{
  mock_flash_driver_io_create(10);
}

TEST_TEAR_DOWN(flash_driver)
{
  mock_flash_driver_io_verify_complete();
  mock_flash_driver_io_destroy();
}

TEST(flash_driver, check_link_success)
{
  uint8_t check_busy_output_data[] = { 0x05 };
  uint8_t check_busy_input_data[] = { 0x00 };
  uint8_t check_link_output_data[] = { 0x90, 0x0, 0x0, 0x0, 0x0, 0x0 };
  uint8_t check_link_input_data[] = { 0x00, 0x0, 0x0, 0x0, 0xef, 0x17 };

  mock_flash_driver_io_expect_select(true);
  mock_flash_driver_io_expect_write_read(
    check_busy_output_data,
    check_busy_input_data,
    sizeof(check_busy_output_data),
    sizeof(check_busy_input_data)
  );
  mock_flash_driver_io_expect_select(false);

  mock_flash_driver_io_expect_select(true);
  mock_flash_driver_io_expect_write_read(
    check_link_output_data,
    check_link_input_data,
    sizeof(check_link_output_data),
    sizeof(check_link_input_data)
  );
  mock_flash_driver_io_expect_select(false);

  flash_driver_status status = flash_driver_check_link();
  TEST_ASSERT_EQUAL(FLASH_DRIVER_OK, status);
}

TEST(flash_driver, check_link_error)
{
  uint8_t check_busy_output_data[] = { 0x05 };
  uint8_t check_busy_input_data[] = { 0x00 };
  uint8_t check_link_output_data[] = { 0x90, 0x0, 0x0, 0x0, 0x0, 0x0 };
  uint8_t check_link_input_data[] = { 0x00, 0x0, 0x0, 0x0, 0x0, 0x0 };

  mock_flash_driver_io_expect_select(true);
  mock_flash_driver_io_expect_write_read(
    check_busy_output_data,
    check_busy_input_data,
    sizeof(check_busy_output_data),
    sizeof(check_busy_input_data)
  );
  mock_flash_driver_io_expect_select(false);

  mock_flash_driver_io_expect_select(true);
  mock_flash_driver_io_expect_write_read(
    check_link_output_data,
    check_link_input_data,
    sizeof(check_link_output_data),
    sizeof(check_link_input_data)
  );
  mock_flash_driver_io_expect_select(false);

  flash_driver_status status = flash_driver_check_link();
  TEST_ASSERT_EQUAL(FLASH_DRIVER_ERROR, status);
}

TEST(flash_driver, check_link_busy_error)
{
  uint8_t check_busy_output_data[] = { 0x05, 0x0 };
  uint8_t check_busy_input_data[] = { 0x0, 0x01 };

  mock_flash_driver_io_expect_select(true);
  mock_flash_driver_io_expect_write_read(
    check_busy_output_data,
    check_busy_input_data,
    sizeof(check_busy_output_data),
    sizeof(check_busy_input_data)
  );
  mock_flash_driver_io_expect_select(false);

  flash_driver_status status = flash_driver_check_link();
  TEST_ASSERT_EQUAL(FLASH_DRIVER_BUSY, status);
}

TEST(flash_driver, write_success)
{
  uint8_t check_busy_output_data[] = { 0x05 };
  uint8_t check_busy_input_data[] = { 0x00 };
  uint8_t write_enable_output_data[] = { 0x06 };
  uint8_t data[] = { 0x55, 0xaa };
  uint8_t write_output_data[6] = { 0x02, 0x55, 0x0, 0xaa };
  memcpy(write_output_data + 4, data, 2);

  mock_flash_driver_io_expect_select(true);
  mock_flash_driver_io_expect_write_read(
    check_busy_output_data,
    check_busy_input_data,
    sizeof(check_busy_output_data),
    sizeof(check_busy_input_data)
  );
  mock_flash_driver_io_expect_select(false);

  mock_flash_driver_io_expect_select(true);
  mock_flash_driver_io_expect_write(
    write_enable_output_data,
    sizeof(write_enable_output_data)
  );
  mock_flash_driver_io_expect_select(false);

  mock_flash_driver_io_expect_select(true);
  mock_flash_driver_io_expect_write(
    write_output_data,
    sizeof(write_output_data)
  );
  mock_flash_driver_io_expect_select(false);

  flash_driver_status status = flash_driver_write(
    0x5500aa,
    data,
    sizeof(data)
  );
  TEST_ASSERT_EQUAL(FLASH_DRIVER_OK, status);
}

TEST(flash_driver, read_success)
{
  uint8_t check_busy_output_data[] = { 0x05 };
  uint8_t check_busy_input_data[] = { 0x00 };
  uint8_t data[4];
  uint8_t read_output_data[4] = { 0x03, 0x55, 0x0, 0xaa };
  memcpy(read_output_data + 4, data, 2);
  uint8_t read_input_data[8] = { 0x0, 0x0, 0x0, 0x0, 0xaa, 0x55, 0xaa, 0x55 };

  mock_flash_driver_io_expect_select(true);
  mock_flash_driver_io_expect_write_read(
    check_busy_output_data,
    check_busy_input_data,
    sizeof(check_busy_output_data),
    sizeof(check_busy_input_data)
  );
  mock_flash_driver_io_expect_select(false);

  mock_flash_driver_io_expect_select(true);
  mock_flash_driver_io_expect_write_read(
    read_output_data,
    read_input_data,
    sizeof(read_output_data),
    sizeof(read_input_data)
  );
  mock_flash_driver_io_expect_select(false);

  flash_driver_status status = flash_driver_read(
    0x5500aa,
    data,
    sizeof(data)
  );

  TEST_ASSERT_EQUAL_HEX8_ARRAY(
    read_input_data + 4,
    data,
    sizeof(data)
  );
  TEST_ASSERT_EQUAL(FLASH_DRIVER_OK, status);
}
