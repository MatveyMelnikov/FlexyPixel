MODULES_DIR = External

HC06_DRIVER = $(MODULES_DIR)/hc06_driver
LED_PANELS_DRIVER = $(MODULES_DIR)/led_panels_driver
CY15B104Q_DRIVER = $(MODULES_DIR)/CY15B104Q_Driver
RENDER_CONTROLLER = $(MODULES_DIR)/render_controller
AUXILIARY_MODULES = $(MODULES_DIR)/auxiliary_modules
TASK_MANAGER = $(MODULES_DIR)/task_manager
TASK = $(MODULES_DIR)/task_manager/task

TESTS = Y

C_INCLUDES = \
-I$(HC06_DRIVER)/Inc \
-I$(LED_PANELS_DRIVER)/Inc \
-I$(CY15B104Q_DRIVER)/Inc \
-I$(AUXILIARY_MODULES)/Inc \
-I$(TASK_MANAGER)/Inc \
-I$(TASK)/Inc \
-I$(RENDER_CONTROLLER)/Inc

C_SOURCES = \
$(HC06_DRIVER)/Src/hc06_driver.c \
$(LED_PANELS_DRIVER)/Src/led_panels_driver.c \
$(CY15B104Q_DRIVER)/Src/cy15b104q_driver.c \
$(AUXILIARY_MODULES)/Src/list_of_changes.c \
$(AUXILIARY_MODULES)/Src/displays_conf.c \
$(AUXILIARY_MODULES)/Src/frame_buffer.c \
$(TASK)/Src/set_mode_task.c \
$(TASK)/Src/set_config_task.c \
$(TASK)/Src/send_data_task.c \
$(TASK)/Src/set_pixel_task.c \
$(TASK)/Src/set_seq_task.c \
$(TASK)/Src/save_task.c \
$(TASK_MANAGER)/Src/task.c \
$(TASK_MANAGER)/Src/task_manager.c \
$(RENDER_CONTROLLER)/Src/render_controller.c

ifeq ($(TESTS), Y)
include MakefileTest.mk
else

C_SOURCES += \
Core/Src/main.c \
$(HC06_DRIVER)/Src/hc06_io.c \
$(HC06_DRIVER)/Src/hc06_time.c \
$(LED_PANELS_DRIVER)/Src/led_panels_io.c \
$(RENDER_CONTROLLER)/Src/render_controller_io.c \
$(AUXILIARY_MODULES)/Src/debug_output.c

include MakefileTarget.mk                                       
endif