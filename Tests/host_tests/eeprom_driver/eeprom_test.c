#include "unity_fixture.h"
#include "eeprom_driver.h"
#include "mock_eeprom_io.h"
#include <string.h>

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

TEST_GROUP(eeprom_driver);

TEST_SETUP(eeprom_driver)
{
    mock_eeprom_io_create(10);
    //eeprom_driver_create(void);
}

TEST_TEAR_DOWN(eeprom_driver)
{
    //eeprom_driver_destroy(void);
    mock_eeprom_io_verify_complete();
    mock_eeprom_io_destroy();
}

TEST(eeprom_driver, check_link_success)
{
    uint8_t output_data[] = { 0x0, 0x0, 0x55 };
    uint8_t input_data = 0x55;
    mock_eeprom_io_expect_write(output_data, 3); // 0x0, 0x0, 0x55
    mock_eeprom_io_expect_write(output_data, 2); // 0x0, 0x0
    mock_eeprom_io_expect_read_then_return(&input_data, 1);

    eeprom_status status = eeprom_check_link();

    TEST_ASSERT_EQUAL(EEPROM_OK, status);
}

TEST(eeprom_driver, random_write_byte_success)
{
    uint16_t addr = 0x0a55;
    uint8_t output_data[3];
    ADD_ADDR_IN_BYTES(output_data, addr);
    output_data[2] = 0x54;

    mock_eeprom_io_expect_write(output_data, 3); // 0xaa, 0x55, 0x54

    eeprom_status status = eeprom_byte_write(addr, 0x54);

    TEST_ASSERT_EQUAL(EEPROM_OK, status);
}

TEST(eeprom_driver, random_read_byte_success)
{
    uint16_t addr = 0x0b55;
    uint8_t output_data[2];
    ADD_ADDR_IN_BYTES(output_data, addr);
    uint8_t input_data = 0x55;
    uint8_t result_data = 0x0;

    mock_eeprom_io_expect_write(output_data, 2); // 0xaa, 0x55 (address)
    mock_eeprom_io_expect_read_then_return(&input_data, 1);

    eeprom_status status = eeprom_random_byte_read(addr, &result_data);

    TEST_ASSERT_EQUAL(EEPROM_OK, status);
    TEST_ASSERT_EQUAL(input_data, result_data);
}

TEST(eeprom_driver, current_address_read_byte_success)
{
    uint8_t input_data = 0x55;
    uint8_t result_data = 0x0;

    mock_eeprom_io_expect_read_then_return(&input_data, 1);

    eeprom_status status = eeprom_current_address_read(&result_data);

    TEST_ASSERT_EQUAL(EEPROM_OK, status);
    TEST_ASSERT_EQUAL(input_data, result_data);
}

TEST(eeprom_driver, page_8_write_success)
{
    uint16_t addr = 0x0c33;
    uint8_t data[8] = {
        0xff, 0xaa, 0xff, 0x0
    };
    uint8_t output_data[10];
    ADD_ADDR_IN_BYTES(output_data, addr);
    memcpy(output_data + 2, data, sizeof(data));
    
    mock_eeprom_io_expect_write(output_data, sizeof(output_data));

    eeprom_status status = eeprom_page_write(addr, data, sizeof(data));

    TEST_ASSERT_EQUAL(EEPROM_OK, status);
}

TEST(eeprom_driver, page_68_write_success)
{
    uint16_t addr = 0x0533;
    uint8_t data[68] = {
        0x33, 0x22, 0x11, 0xaa
    };
    memcpy(data, data, 4);
    memcpy(data + 64, data, 4);

    uint8_t output_data_0[13];
    ADD_ADDR_IN_BYTES(output_data_0, addr);
    memcpy(output_data_0 + 2, data, 13);

    uint8_t output_data_1[55];
    ADD_ADDR_IN_BYTES(output_data_1, addr + 13);
    memcpy(output_data_1 + 2, data + 13, 55);
    
    mock_eeprom_io_expect_write(output_data_0, sizeof(output_data_0));
    mock_eeprom_io_expect_write(output_data_1, sizeof(output_data_1));
    
    eeprom_status status = eeprom_page_write(addr, data, sizeof(data));

    TEST_ASSERT_EQUAL(EEPROM_OK, status);
}

TEST(eeprom_driver, page_128_write_success)
{
    uint16_t addr = 0x088;
    uint8_t data[128] = {
        0xff, 0xaa, 0xff, 0x0
    };
    memcpy(data + 40, data, 4);
    memcpy(data + 80, data, 4);
    memcpy(data + 100, data, 4);
    memcpy(data + 124, data, 4);

    uint8_t output_data_0[58];
    ADD_ADDR_IN_BYTES(output_data_0, addr);
    memcpy(output_data_0 + 2, data, 56);

    uint8_t output_data_1[66];
    ADD_ADDR_IN_BYTES(output_data_1, addr + 56);
    memcpy(output_data_1 + 2, data + 56, 64);

    uint8_t output_data_2[10];
    ADD_ADDR_IN_BYTES(output_data_2, addr + 120);
    memcpy(output_data_2 + 2, data + 120, 8);
    
    mock_eeprom_io_expect_write(output_data_0, sizeof(output_data_0));
    mock_eeprom_io_expect_write(output_data_1, sizeof(output_data_1));
    mock_eeprom_io_expect_write(output_data_2, sizeof(output_data_2));
    
    eeprom_status status = eeprom_page_write(addr, data, sizeof(data));

    TEST_ASSERT_EQUAL(EEPROM_OK, status);
}

TEST(eeprom_driver, page_133_write_success)
{
    uint16_t addr = 0x0178;
    uint8_t data[133] = {
        0x22, 0x11, 0x66, 0x55
    };
    memcpy(data + 129, data, 4);
    memcpy(data + 100, data, 4);
    memcpy(data + 65, data, 4);

    uint8_t output_data_0[10];
    ADD_ADDR_IN_BYTES(output_data_0, addr);
    memcpy(output_data_0 + 2, data, 8);

    uint8_t output_data_1[66];
    ADD_ADDR_IN_BYTES(output_data_1, addr + 8);
    memcpy(output_data_1 + 2, data + 8, 64);

    uint8_t output_data_2[63];
    ADD_ADDR_IN_BYTES(output_data_2, addr + 72);
    memcpy(output_data_2 + 2, data + 72, 61);

    mock_eeprom_io_expect_write(output_data_0, sizeof(output_data_0));
    mock_eeprom_io_expect_write(output_data_1, sizeof(output_data_1));
    mock_eeprom_io_expect_write(output_data_2, sizeof(output_data_2));
    
    eeprom_status status = eeprom_page_write(addr, data, sizeof(data));

    TEST_ASSERT_EQUAL(EEPROM_OK, status);
}

TEST(eeprom_driver, page_aligned_write_success)
{
    uint16_t addr = 0x1c0;
    uint8_t data[64] = { 0xff, 0xaa, 0xff, 0x0 };
    memcpy(data + 60, data, 4);
    uint8_t output_data[66];
    ADD_ADDR_IN_BYTES(output_data, addr);
    memcpy(output_data + 2, data, sizeof(data));
    
    mock_eeprom_io_expect_write(output_data, sizeof(output_data));

    eeprom_status status = eeprom_aligned_page_write(addr, data);

    TEST_ASSERT_EQUAL(EEPROM_OK, status);
}

TEST(eeprom_driver, page_aligned_write_error)
{
    uint16_t addr = 0x1c1;
    uint8_t data[64] = { 0xff, 0xaa, 0xff, 0x0 };
    memcpy(data + 60, data, 4);
    
    eeprom_status status = eeprom_aligned_page_write(addr, data);

    TEST_ASSERT_EQUAL(EEPROM_ERROR, status);
}
