# Building tests to run on the development system (host)
# In this file you can add files that will be tested on a specific platform

CC = gcc
FLAGS = -std=c99
BUILD_DIR = $(UNITY_DIR)/build
TARGET = $(BUILD_DIR)/tests.out
TESTS_DIR = Tests

FLAGS += \
-g3

C_INCLUDES += \
-ICore/Inc

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))

all: $(TARGET)

vpath %.c $(dir $(C_SOURCES))

$(BUILD_DIR)/%.o: %.c
	$(CC) $(FLAGS) $(CFLAGS) -MD $(C_INCLUDES) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o $(TARGET)

.PHONY = start
start: $(TARGET)
	./$(TARGET) -v # -v - print tests

.PHONY = clean
clean:
	rm -f $(BUILD_DIR)/*

-include $(OBJECTS:.o=.d)