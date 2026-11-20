#
# Makefile for anyka
# Nov. 2024
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)

PROJ = iconv
VERSION := $(CONFIG_THIRD_PARTY_ICONV_VERSION)

MODULE_ROOT ?= $(shell pwd)
# Use BUILD_DIR passed from parent Makefile, fallback to local build directory
BUILD_DIR ?= $(MODULE_ROOT)/build

CONFIG_OPTION := --enable-static=yes --enable-shared=yes --host=$(CROSS_TOOLCHAIN) --prefix=$(BUILD_DIR)  CFLAGS=-fPIC


CFLAGS += -Werror

##build rule
.PHONY: all  clean

all: vendor_tree
	@# Out-of-tree build: configure and make in BUILD_DIR (sources under $(WORK_SRC))
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(WORK_SRC)/configure $(CONFIG_OPTION)
	cd $(BUILD_DIR) && make
	cd $(BUILD_DIR) && make install

	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib
	cp -rf  $(BUILD_DIR)/lib/*.a $(INSTALL_DIR)/$(PROJ)/lib
	cp -rf  $(BUILD_DIR)/lib/*.so* $(INSTALL_DIR)/$(PROJ)/lib
	cp -rf  $(BUILD_DIR)/include $(INSTALL_DIR)/$(PROJ)/

clean:
	$(RM) -rf $(BUILD_DIR)


