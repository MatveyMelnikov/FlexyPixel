# Building tests to run on the development system (host)

CC = gcc
FLAGS = -std=c99
BUILD_DIR = $(UNITY_DIR)/build
TARGET = $(BUILD_DIR)/tests.out

FLAGS += \
-g3

C_INCLUDES += \
-ICore/Inc

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))

all: $(TARGET)

vpath %.c $(dir $(C_SOURCES))

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(FLAGS) $(CFLAGS) -MD $(C_INCLUDES) -c $< -o $@

$(TARGET): $(OBJECTS) | $(BUILD_DIR)
	$(CC) $(FLAGS) $(OBJECTS) -o $(TARGET) -lm

$(BUILD_DIR):
	mkdir $@	

.PHONY = start
start: $(TARGET)
	./$(TARGET) -v # -v - print tests

.PHONY = clean
clean:
	-rm -fR $(BUILD_DIR)

-include $(OBJECTS:.o=.d)