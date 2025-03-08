CFLAGS = -DTEST -DUNITY_INCLUDE_CONFIG_H
TESTS_DIR = Tests
HOST_TESTS = $(TESTS_DIR)/host_tests
MOCKS_DIR = $(TESTS_DIR)/mocks
FAKES_DIR = $(TESTS_DIR)/fakes
UNITY_DIR = $(MODULES_DIR)/Unity-2.5.2

FOR_TARGET = N

HOST_TEST_HC06_DRIVER_DIR = $(HOST_TESTS)/hc06_driver
HOST_TEST_LED_PANELS_DRIVER_DIR = $(HOST_TESTS)/led_panels_driver
HOST_TEST_RENDER_CONTROLLER_DIR = $(HOST_TESTS)/render_controller

# TARGET_TEST_FLASH_DRIVER_DIR = ...

C_INCLUDES += \
-I$(UNITY_DIR)/src \
-I$(UNITY_DIR)/extras/fixture/src \
-I$(UNITY_DIR)/extras/memory/src \
-I$(TESTS_DIR) \
-I$(MOCKS_DIR)/Inc \
-I$(FAKES_DIR)/Inc

C_SOURCES += \
$(UNITY_DIR)/src/unity.c \
$(UNITY_DIR)/extras/fixture/src/unity_fixture.c \
$(UNITY_DIR)/extras/memory/src/unity_memory.c

ifeq ($(FOR_TARGET), Y)
# ...
include MakefileTarget.mk
else
C_SOURCES += \
$(TESTS_DIR)/host_tests.c \
$(HOST_TEST_HC06_DRIVER_DIR)/hc06_test_runner.c \
$(HOST_TEST_HC06_DRIVER_DIR)/hc06_test.c \
$(MOCKS_DIR)/Src/mock.c \
$(MOCKS_DIR)/Src/mock_delay.c \
$(MOCKS_DIR)/Src/mock_uart.c \
$(HOST_TEST_LED_PANELS_DRIVER_DIR)/led_panels_test_runner.c \
$(HOST_TEST_LED_PANELS_DRIVER_DIR)/led_panels_test.c
# $(MOCKS_DIR)/Src/mock_render_controller_io.c \
# $(FAKES_DIR)/Src/fake_debug_output.c \
# $(FAKES_DIR)/Src/fake_hc06_time.c \
# $(FAKES_DIR)/Src/fake_led_panels_io.c
# $(MOCKS_DIR)/Src/mock_hc06_io.c \
# $(HOST_TEST_RENDER_CONTROLLER_DIR)/render_controller_test_runner.c \
# $(HOST_TEST_RENDER_CONTROLLER_DIR)/render_controller_test.c \

include MakefileTestHost.mk
endif
