# File for test cases
# Here you can add platform-specific files (main file, test, etc.)

CFLAGS = -DTEST -DUNITY_INCLUDE_CONFIG_H
TESTS_DIR = Tests
UNITY_DIR = External/Unity-2.5.2
HC06_DRIVER = External/hc06_driver
LED_PANELS_DRIVER = External/led_panels_driver
RENDER_CONTROLLER = External/render_controller
AUXILIARY_MODULES = External/auxiliary_modules
HANDLER = External/handler

C_INCLUDES += \
-I$(HC06_DRIVER)/Inc \
-I$(LED_PANELS_DRIVER)/Inc \
-I$(UNITY_DIR)/src \
-I$(UNITY_DIR)/extras/fixture/src \
-I$(UNITY_DIR)/extras/memory/src \
-I$(RENDER_CONTROLLER)/Inc \
-I$(AUXILIARY_MODULES)/Inc \
-I$(HANDLER)/Inc \
-I$(RENDER_CONTROLLER)/Inc \
-I$(TESTS_DIR)

C_SOURCES += \
$(HC06_DRIVER)/Src/hc06_driver.c \
$(LED_PANELS_DRIVER)/Src/led_panels_driver.c \
$(UNITY_DIR)/src/unity.c \
$(UNITY_DIR)/extras/fixture/src/unity_fixture.c \
$(UNITY_DIR)/extras/memory/src/unity_memory.c \
$(AUXILIARY_MODULES)/Src/list_of_changes.c \
$(AUXILIARY_MODULES)/Src/displays_conf.c \
$(AUXILIARY_MODULES)/Src/operation_mode.c \
$(HANDLER)/Src/pixel_mode_handler.c \
$(HANDLER)/Src/handler.c \
$(HANDLER)/Src/handler_queue.c \
$(RENDER_CONTROLLER)/Src/render_controller.c


C_INCLUDES += \
-I$(TESTS_DIR)/mocks/Inc \
-I$(TESTS_DIR)/fakes/Inc
C_SOURCES += \
$(TESTS_DIR)/host_tests.c \
$(TESTS_DIR)/host_tests/hc06_driver/hc06_test_runner.c \
$(TESTS_DIR)/host_tests/hc06_driver/hc06_test.c \
$(TESTS_DIR)/mocks/Src/mock_hc06_io.c \
$(TESTS_DIR)/fakes/Src/fake_hc06_time.c \
$(TESTS_DIR)/host_tests/led_panels_driver/led_panels_test_runner.c \
$(TESTS_DIR)/host_tests/led_panels_driver/led_panels_test.c \
$(TESTS_DIR)/mocks/Src/mock_led_panels_io.c \
$(TESTS_DIR)/mocks/Src/mock_render_controller_io.c \
$(TESTS_DIR)/host_tests/render_controller/render_controller_test_runner.c \
$(TESTS_DIR)/host_tests/render_controller/render_controller_test.c

include MakefileTestHost.mk
