#!/bin/bash

# define toolchain (should be arm compilers or compile on the jetson)
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

LDFLAGS += -lv4l2 -lpthread
CFLAGS += -g

# default rule
all: $(TARGET)

# link step
$(TARGET): $(OBJS)
	$(LD) $(OBJS) -o $@ $(LDFLAGS)

# build step
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -r $(BUILD_DIR)/*
	rm $(TARGET)
