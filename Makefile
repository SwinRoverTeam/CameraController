#!/bin/bash

# define toolchain (should be same on jetson)
CC := gcc
LD := gcc

# options
TARGET := cam_ctrl

# define directories
SRC_DIR := ./src
BUILD_DIR := ./obj

# find files
SRC := $(shell find -wholename '$(SRC_DIR)/*.c')
OBJS := $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := $(addprefix -I, $(INC_DIRS))

LDFLAGS += -lc -lv4l2 -lSDL2
CFLAGS += -g $(INC_FLAGS)

# default rule
all: $(shell mkdir -p ./obj) $(TARGET)

# link step
$(TARGET): $(OBJS)
	$(LD) $(OBJS) -o $@ $(LDFLAGS)

# build step
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean tests

clean:
	rm -r $(BUILD_DIR)
	rm $(TARGET)

tests:
	#gcc -g test/device_test.c src/video_capture.c -lc -o test/dev
	gcc -g test/log_test.c -DDEBUG -lc -o test/log
	gcc -g test/panel_test.c src/panels.c -lc -o test/panel
	gcc -g test/serial_test.c src/serial_ports.c -lc -o test/serial
	gcc -g test/stream_test.c src/video_capture.c src/draw_frame.c -lSDL2 -o test/stream

clean_test:
	rm test/stream
	rm test/log
	rm test/serial
