#include "unity_fixture.h"

TEST_GROUP_RUNNER(eeprom_driver)
{
    RUN_TEST_CASE(eeprom_driver, check_link_success);
    RUN_TEST_CASE(eeprom_driver, random_write_byte_success);
    RUN_TEST_CASE(eeprom_driver, random_read_byte_success);
    RUN_TEST_CASE(eeprom_driver, current_address_read_byte_success);
    RUN_TEST_CASE(eeprom_driver, page_8_write_success);
    RUN_TEST_CASE(eeprom_driver, page_68_write_success);
    RUN_TEST_CASE(eeprom_driver, page_128_write_success);
    RUN_TEST_CASE(eeprom_driver, page_133_write_success);
    RUN_TEST_CASE(eeprom_driver, page_aligned_write_success);
    RUN_TEST_CASE(eeprom_driver, page_aligned_write_error);
}