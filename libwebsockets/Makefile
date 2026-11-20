#
# Makefile for libwebsockets
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)

export CROSS_COMPILE

# Use absolute cross compiler paths so CMake does not pick a broken wrapper in
# /usr/local/bin via PATH (same pattern as third_party/lvgl/Makefile).
CROSS_CC  := $(firstword $(wildcard $(CROSS_PATH)/bin/$(CROSS_COMPILE)gcc) $(CROSS_COMPILE)gcc)
CROSS_CXX := $(firstword $(wildcard $(CROSS_PATH)/bin/$(CROSS_COMPILE)g++) $(CROSS_COMPILE)g++)

# ============================================================================
# Variables
# ============================================================================

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)
PROJ = libwebsockets
VERSION = $(CONFIG_THIRD_PARTY_WSS_VERSION)

MODULE_ROOT ?= $(shell pwd)
# BUILD_DIR can be passed from parent Makefile (e.g., out/build/third_party/libwebsockets)
BUILD_DIR ?= $(MODULE_ROOT)/build

SRC_DIR := $(MODULE_ROOT)/$(PROJ)-$(VERSION)
BUILD_SCRIPT := $(MODULE_ROOT)/build_make.sh

# ============================================================================
# Targets
# ============================================================================

.PHONY: all target clean install

all: target install

target:
	mkdir -p $(BUILD_DIR)
	@# Use absolute path to call build_make.sh to ensure it can be found when BUILD_DIR
	@# is something like out/build/third_party/libwebsockets
	@cd $(BUILD_DIR) && bash $(BUILD_SCRIPT) $(SRC_DIR) $(CROSS_COMPILE) $(THIRD_PARTY_INSTALL_DIR) $(CROSS_CC) $(CROSS_CXX)
	@cd $(BUILD_DIR) && make

# Install targets
install: target
	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib
	cp $(BUILD_DIR)/lib/*.a $(INSTALL_DIR)/$(PROJ)/lib
	cp $(BUILD_DIR)/lib/*.so* $(INSTALL_DIR)/$(PROJ)/lib 2>/dev/null || true
	cp $(BUILD_DIR)/include $(INSTALL_DIR)/$(PROJ)/ -rf

# Clean targets
clean:
	@echo "Cleaning libwebsockets build files..."
	$(RM) -rf $(BUILD_DIR)
