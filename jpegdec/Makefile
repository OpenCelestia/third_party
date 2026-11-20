#
# Makefile for jpegdec
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)

# ============================================================================
# Variables
# ============================================================================

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)
PROJ = jpegdec
VERSION = $(CONFIG_THIRD_PARTY_JPEGDEC_VERSION)

MODULE_ROOT ?= $(shell pwd)
# BUILD_DIR can be passed from parent Makefile (e.g., out/build/third_party/jpegdec)
BUILD_DIR ?= $(MODULE_ROOT)/build

# Source files discovery (align with cjson style)
SRC_ROOT = $(MODULE_ROOT)/$(PROJ)-$(VERSION)
SRC_DIR = $(shell find $(SRC_ROOT) -maxdepth 3 -type d)
INCLUDE += $(foreach path, $(SRC_DIR), -I$(path))

CSRCS = $(foreach d, $(SRC_DIR), $(wildcard $d/*.c))
CXXSRCS = $(foreach d, $(SRC_DIR), $(wildcard $d/*.cpp))

# Object files - all output to BUILD_DIR
OBJS = $(patsubst $(SRC_ROOT)/%.c,$(BUILD_DIR)/%.o,$(CSRCS))
OBJS += $(patsubst $(SRC_ROOT)/%.cpp,$(BUILD_DIR)/%.o,$(CXXSRCS))

# Library names
STATIC_LIB := lib$(PROJ).a
DYNAMIC_LIB := lib$(PROJ).so

# Build targets
TARGET := $(BUILD_DIR)/$(STATIC_LIB)
ifeq ($(CONFIG_OS_NAME), linux)
TARGET += $(BUILD_DIR)/$(DYNAMIC_LIB)
endif

# Compiler flags
CFLAGS += -Werror
# JPEGDEC.h switches away from Arduino.h when __LINUX__ is defined.
CFLAGS += -D__LINUX__
CXXFLAGS += -D__LINUX__

# ============================================================================
# Targets
# ============================================================================

.PHONY: all target clean install

all: target install

target: $(TARGET)
$(BUILD_DIR)/$(STATIC_LIB): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(AR) -rsv $@ $(OBJS)

ifeq ($(CONFIG_OS_NAME), linux)
$(BUILD_DIR)/$(DYNAMIC_LIB): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) $(CFLAGS) -shared -fPIC -o $@ $(LDFLAGS)
endif

# Compile C source files to object files
$(BUILD_DIR)/%.o: $(SRC_ROOT)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# Compile C++ source files to object files
$(BUILD_DIR)/%.o: $(SRC_ROOT)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

# Install targets
install: $(TARGET)
	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib
	cp -f $(TARGET) $(INSTALL_DIR)/$(PROJ)/lib
ifeq ($(CONFIG_OS_NAME), linux)
	@true
endif
	cp -rf $(SRC_ROOT)/include $(INSTALL_DIR)/$(PROJ)

# Clean targets
clean:
	@echo "Cleaning jpegdec build files..."
	$(RM) -rf $(BUILD_DIR)
