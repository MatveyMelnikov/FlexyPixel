MODULES_DIR = External

DOMAIN = $(MODULES_DIR)/Domain
INFRASTRUCTURE = $(MODULES_DIR)/Infrastructure
BUILDERS = $(INFRASTRUCTURE)/Builders
PORTS = $(DOMAIN)/Ports

# HC06_DRIVER = $(MODULES_DIR)/hc06_driver
# LED_PANELS_DRIVER = $(MODULES_DIR)/led_panels_driver
# CY15B104Q_DRIVER = $(MODULES_DIR)/CY15B104Q_Driver
# RENDER_CONTROLLER = $(MODULES_DIR)/render_controller
# AUXILIARY_MODULES = $(MODULES_DIR)/auxiliary_modules

TASK = $(DOMAIN)/Task
TASK_CLUSTER = $(DOMAIN)/Task_Cluster
HASH_TABLE = $(DOMAIN)/Hash_Table
TASK_MANAGER = $(DOMAIN)/Task_Manager
TASK_CLUSTER_RECEIVE_CMD = $(DOMAIN)/Task_Clusters/Task_Cluster_Receive_CMD
TASK_CLUSTER_SET_CONFIG = $(DOMAIN)/Task_Clusters/Task_Cluster_Set_Config
TASK_CLUSTER_HEART_BEAT = $(DOMAIN)/Task_Clusters/Task_Cluster_Heart_Beat
TASK_CLUSTER_STARTUP = $(DOMAIN)/Task_Clusters/Task_Cluster_Startup
TASK_CLUSTER_RENDER = $(DOMAIN)/Task_Clusters/Task_Cluster_Render
TASK_CLUSTER_SET_MODE = $(DOMAIN)/Task_Clusters/Task_Cluster_Set_Mode
TASK_CLUSTER_RECEIVE_DATA = $(DOMAIN)/Task_Clusters/Task_Cluster_Receive_Data
TASK_CLUSTER_PIX_DATA = $(DOMAIN)/Task_Clusters/Task_Cluster_Pix_Data
TASK_CLUSTER_SAVE = $(DOMAIN)/Task_Clusters/Task_Cluster_Save
TASK_CLUSTER_SEQ_PARAMS = $(DOMAIN)/Task_Clusters/Task_Cluster_Seq_Params
TASK_CLUSTER_SEQ_DATA = $(DOMAIN)/Task_Clusters/Task_Cluster_Seq_Data
TASK_CLUSTER_RESET = $(DOMAIN)/Task_Clusters/Task_Cluster_Reset

DISPLAYS_CONFIG_REPO = $(DOMAIN)/Displays_Config_Repo
SINGLE_CHANGES_REPO = $(DOMAIN)/Single_Changes_Repo
FRAMES_REPO = $(DOMAIN)/Frames_Repo
RENDER_CONTROLLER = $(DOMAIN)/Render_Controller
MESSAGE_HANDLER = $(DOMAIN)/Message_Handler
MODE_REPO = $(DOMAIN)/Mode_Repo
PACKED_PIXEL_DATA = $(DOMAIN)/Packed_Pixel_Data
DEBUG_HANDLER = $(DOMAIN)/Debug_Handler

DATA_TRANSMITTER_PORT = $(PORTS)/Data_Transmitter_Port

HC06_DRIVER = $(INFRASTRUCTURE)/HC06_Driver
CY15B104Q_DRIVER = $(INFRASTRUCTURE)/CY15B104Q_Driver
LED_PANELS_DRIVER = $(INFRASTRUCTURE)/LED_Panels_Driver
DISPLAYS_CONFIG_STORAGE = $(INFRASTRUCTURE)/Displays_Config_Storage
SINGLE_CHANGES_STORAGE = $(INFRASTRUCTURE)/Single_Changes_Storage
FRAMES_STORAGE = $(INFRASTRUCTURE)/Frames_Storage

BUILDER_HC06_DRIVER = $(BUILDERS)/Builder_HC06_Driver
BUILDER_CY15B104Q_DRIVER = $(BUILDERS)/Builder_CY15B104Q_Driver
BUILDER_LED_PANELS_DRIVER = $(BUILDERS)/Builder_Led_Panels_Driver
BUILDER_DATA_TRANSMITTER_PORT = $(BUILDERS)/Builder_Data_Transmitter_Port
BUILDER_TASK_MANAGER = $(BUILDERS)/Builder_Task_Manager
BUILDER_DEBUG_HANDLER = $(BUILDERS)/Builder_Debug_Handler
BUILDER_GENERAL = $(BUILDERS)/Builder_General

TESTS = N

# C_INCLUDES = \
# -I$(HC06_DRIVER)/Inc \
# -I$(LED_PANELS_DRIVER)/Inc \
# -I$(CY15B104Q_DRIVER)/Inc \
# -I$(AUXILIARY_MODULES)/Inc \
# -I$(TASK_MANAGER)/Inc \
# -I$(TASK)/Inc \
# -I$(RENDER_CONTROLLER)/Inc

C_INCLUDES = \
-I$(TASK)/Inc \
-I$(TASK_CLUSTER)/Inc \
-I$(HASH_TABLE)/Inc \
-I$(TASK_MANAGER)/Inc \
-I$(TASK_CLUSTER_RECEIVE_CMD)/Inc \
-I$(TASK_CLUSTER_SET_CONFIG)/Inc \
-I$(TASK_CLUSTER_HEART_BEAT)/Inc \
-I$(TASK_CLUSTER_STARTUP)/Inc \
-I$(TASK_CLUSTER_RENDER)/Inc \
-I$(TASK_CLUSTER_SET_MODE)/Inc \
-I$(TASK_CLUSTER_RECEIVE_DATA)/Inc \
-I$(TASK_CLUSTER_PIX_DATA)/Inc \
-I$(TASK_CLUSTER_SAVE)/Inc \
-I$(TASK_CLUSTER_SEQ_PARAMS)/Inc \
-I$(TASK_CLUSTER_SEQ_DATA)/Inc \
-I$(TASK_CLUSTER_RESET)/Inc \
-I$(DISPLAYS_CONFIG_REPO)/Inc \
-I$(SINGLE_CHANGES_REPO)/Inc \
-I$(FRAMES_REPO)/Inc \
-I$(RENDER_CONTROLLER)/Inc \
-I$(MESSAGE_HANDLER)/Inc \
-I$(MODE_REPO)/Inc \
-I$(PACKED_PIXEL_DATA)/Inc \
-I$(DEBUG_HANDLER)/Inc \
-I$(DATA_TRANSMITTER_PORT)/Inc \
-I$(HC06_DRIVER)/Inc \
-I$(CY15B104Q_DRIVER)/Inc \
-I$(LED_PANELS_DRIVER)/Inc \
-I$(DISPLAYS_CONFIG_STORAGE)/Inc \
-I$(SINGLE_CHANGES_STORAGE)/Inc \
-I$(FRAMES_STORAGE)/Inc


# C_SOURCES = \
# $(HC06_DRIVER)/Src/hc06_driver.c \
# $(LED_PANELS_DRIVER)/Src/led_panels_driver.c \
# $(CY15B104Q_DRIVER)/Src/cy15b104q_driver.c \
# $(AUXILIARY_MODULES)/Src/list_of_changes.c \
# $(AUXILIARY_MODULES)/Src/displays_conf.c \
# $(AUXILIARY_MODULES)/Src/frame_buffer.c \
# $(TASK)/Src/set_mode_task.c \
# $(TASK)/Src/set_config_task.c \
# $(TASK)/Src/send_data_task.c \
# $(TASK)/Src/set_pixel_task.c \
# $(TASK)/Src/set_seq_task.c \
# $(TASK)/Src/save_task.c \
# $(TASK_MANAGER)/Src/task.c \
# $(TASK_MANAGER)/Src/task_manager.c \
# $(RENDER_CONTROLLER)/Src/render_controller.c

C_SOURCES = \
$(TASK)/Src/task.c \
$(TASK_CLUSTER)/Src/task_cluster.c \
$(HASH_TABLE)/Src/hash_table.c \
$(TASK_MANAGER)/Src/task_manager.c \
$(TASK_CLUSTER_RECEIVE_CMD)/Src/task_cluster_receive_cmd.c \
$(TASK_CLUSTER_SET_CONFIG)/Src/task_cluster_set_config.c \
$(TASK_CLUSTER_HEART_BEAT)/Src/task_cluster_heart_beat.c \
$(TASK_CLUSTER_STARTUP)/Src/task_cluster_startup.c \
$(TASK_CLUSTER_RENDER)/Src/task_cluster_render.c \
$(TASK_CLUSTER_SET_MODE)/Src/task_cluster_set_mode.c \
$(TASK_CLUSTER_RECEIVE_DATA)/Src/task_cluster_receive_data.c \
$(TASK_CLUSTER_PIX_DATA)/Src/task_cluster_pix_data.c \
$(TASK_CLUSTER_SAVE)/Src/task_cluster_save.c \
$(TASK_CLUSTER_SEQ_PARAMS)/Src/task_cluster_seq_params.c \
$(TASK_CLUSTER_SEQ_DATA)/Src/task_cluster_seq_data.c \
$(TASK_CLUSTER_RESET)/Src/task_cluster_reset.c \
$(DISPLAYS_CONFIG_REPO)/Src/displays_config_repo.c \
$(SINGLE_CHANGES_REPO)/Src/single_changes_repo.c \
$(FRAMES_REPO)/Src/frames_repo.c \
$(RENDER_CONTROLLER)/Src/render_controller.c \
$(MESSAGE_HANDLER)/Src/message_handler.c \
$(MODE_REPO)/Src/mode_repo.c \
$(PACKED_PIXEL_DATA)/Src/packed_pixel_data.c \
$(DEBUG_HANDLER)/Src/debug_handler.c \
$(DATA_TRANSMITTER_PORT)/Src/data_transmitter_port.c \
$(HC06_DRIVER)/Src/hc06_driver.c \
$(CY15B104Q_DRIVER)/Src/cy15b104q_driver.c \
$(LED_PANELS_DRIVER)/Src/led_panels_driver.c \
$(DISPLAYS_CONFIG_STORAGE)/Src/displays_config_storage.c \
$(SINGLE_CHANGES_STORAGE)/Src/single_changes_storage.c \
$(FRAMES_STORAGE)/Src/frames_storage.c

ifeq ($(TESTS), Y)
include MakefileTest.mk
else

# C_SOURCES += \
# Core/Src/main.c \
# $(HC06_DRIVER)/Src/hc06_io.c \
# $(HC06_DRIVER)/Src/hc06_time.c \
# $(LED_PANELS_DRIVER)/Src/led_panels_io.c \
# $(RENDER_CONTROLLER)/Src/render_controller_io.c \
# $(AUXILIARY_MODULES)/Src/debug_output.c

C_INCLUDES += \
-I$(BUILDER_HC06_DRIVER)/Inc \
-I$(BUILDER_CY15B104Q_DRIVER)/Inc \
-I$(BUILDER_LED_PANELS_DRIVER)/Inc \
-I$(BUILDER_DATA_TRANSMITTER_PORT)/Inc \
-I$(BUILDER_TASK_MANAGER)/Inc \
-I$(BUILDER_DEBUG_HANDLER)/Inc \
-I$(BUILDER_GENERAL)/Inc

C_SOURCES += \
Core/Src/main.c \
$(BUILDER_HC06_DRIVER)/Src/builder_hc06_driver.c \
$(BUILDER_CY15B104Q_DRIVER)/Src/builder_cy15b104q_driver.c \
$(BUILDER_LED_PANELS_DRIVER)/Src/builder_led_panels_driver.c \
$(BUILDER_DATA_TRANSMITTER_PORT)/Src/builder_data_transmitter_port.c \
$(BUILDER_TASK_MANAGER)/Src/builder_task_manager.c \
$(BUILDER_DEBUG_HANDLER)/Src/builder_debug_handler.c \
$(BUILDER_GENERAL)/Src/builder_general.c


include MakefileTarget.mk                                       
endif