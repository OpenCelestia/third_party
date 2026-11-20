#
# Makefile for anyka
# Nov. 2024
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)
PROJ := jerryscript
VERSION := $(CONFIG_THIRD_PARTY_JERRYSCRIPT_VERSION)

PORT_NAME := jerry_port_default
CMAKE_FILE := toolchain-anyka-${CONFIG_OS_NAME}.cmake

TARGET :=
MODULE_ROOT ?= $(shell pwd)

SRC_DIR := $(MODULE_ROOT)/$(PROJ)-$(VERSION)
PORT_DIR := $(MODULE_ROOT)/$(PORT_NAME)
# Use BUILD_DIR passed from parent Makefile, fallback to local build directory
BUILD_DIR ?= $(MODULE_ROOT)/build

TARGET += $(INSTALL_DIR)/$(PROJ)/lib

$(shell mkdir -p $(BUILD_DIR))

TOOLCHAIN := --toolchain=cmake/$(CMAKE_FILE)

#KM02的CROSS_TOOLCHAIN无法编译通过，后续确认
CONFIG_OPTION := --prefix=$(BUILD_DIR) --host=$(CROSS_TOOLCHAIN) --lto=off --jerry-cmdline=off --jerry-port-default=on --jerry-math=ON --js-parser=on --logging=on --line-info=on --error-messages=on

ifeq ($(CONFIG_OS_NAME),linux)
	CONFIG_OPTION += --shared-libs=on
endif

##build rule
.PHONY: all clean

all: vendor_tree
	cd $(WORK_SRC) && python tools/build.py $(TOOLCHAIN) $(CONFIG_OPTION)
	mkdir -p $(INSTALL_DIR)/$(PROJ)/include
	cp -rf  $(BUILD_DIR)/lib $(INSTALL_DIR)/$(PROJ)
	cp -urf  $(BUILD_DIR)/*/*.h $(INSTALL_DIR)/$(PROJ)/include
	cp -rf  $(WORK_SRC)/jerry-ext/include/jerryscript-ext/handler.h $(INSTALL_DIR)/$(PROJ)/include
	cp -rf  $(WORK_SRC)/jerry-ext/include/jerryscript-ext/module.h $(INSTALL_DIR)/$(PROJ)/include

clean:
	$(RM)	-rf  $(BUILD_DIR)
