#
# Makefile for minIni
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)

# ============================================================================
# Variables
# ============================================================================

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)
PROJ = minIni
VERSION = 1.7.15

MODULE_ROOT ?= $(shell pwd)
# BUILD_DIR can be passed from parent Makefile (e.g., out/build/third_party/minIni)
BUILD_DIR ?= $(MODULE_ROOT)/build

# Source files discovery
SRCDIR = $(shell find $(MODULE_ROOT) -maxdepth 3 -type d)
INCLUDE += $(foreach path, $(SRCDIR), -I$(path))

CSRCS = $(foreach d, $(SRCDIR), $(wildcard $d/*.c))
CXXSRCS = $(foreach d, $(SRCDIR), $(wildcard $d/*.cpp))

# Object files - all output to BUILD_DIR
OBJS = $(patsubst $(MODULE_ROOT)/%.c,$(BUILD_DIR)/%.o,$(CSRCS))
OBJS += $(patsubst $(MODULE_ROOT)/%.cpp,$(BUILD_DIR)/%.o,$(CXXSRCS))

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
$(BUILD_DIR)/%.o: $(MODULE_ROOT)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# Compile C++ source files to object files
$(BUILD_DIR)/%.o: $(MODULE_ROOT)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

# Install targets
install: $(TARGET)
	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib
	cp -f $(BUILD_DIR)/$(STATIC_LIB) $(INSTALL_DIR)/$(PROJ)/lib
ifeq ($(CONFIG_OS_NAME), linux)
	cp -f $(BUILD_DIR)/$(DYNAMIC_LIB) $(INSTALL_DIR)/$(PROJ)/lib
endif
	cp -rf $(MODULE_ROOT)/include $(INSTALL_DIR)/$(PROJ)

# Clean targets
clean:
	@echo "Cleaning minIni build files..."
	$(RM) -rf $(BUILD_DIR)