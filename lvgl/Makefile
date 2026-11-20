#
# Makefile for anyka
# Nov. 2024
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)
export CROSS_COMPILE
# export CONFIG_OS_NAME
# lvgl third_party: ignores project CONFIG_OS_NAME; CMake always configured for Linux (flags / include / libs).
INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)

PROJ = lvgl
VERSION = $(CONFIG_THIRD_PARTY_LVGL_VERSION)

MODULE_ROOT ?= $(shell pwd)
SRC_DIR := $(MODULE_ROOT)/$(PROJ)-$(VERSION)
# Use BUILD_DIR passed from parent Makefile, fallback to local build directory
BUILD_DIR ?= $(MODULE_ROOT)/build

CFLAGS += -Werror

# Use the absolute cross compiler under CROSS_PATH (/opt/<toolchain>) so CMake does not
# resolve the bare name via PATH — a stray/broken riscv gcc wrapper in /usr/local/bin
# (missing its .br_real) would otherwise shadow the real toolchain. Fall back to the
# bare name only if the absolute path is absent.
CROSS_CC  := $(firstword $(wildcard $(CROSS_PATH)/bin/$(CROSS_COMPILE)gcc) $(CROSS_COMPILE)gcc)
CROSS_CXX := $(firstword $(wildcard $(CROSS_PATH)/bin/$(CROSS_COMPILE)g++) $(CROSS_COMPILE)g++)

.PHONY: all target install clean

all: target install

target:
	@if [ ! -d $(SRC_DIR) ] ; then tar xvf $(SRC_DIR).tar.* ; fi
	@if [ -z "$(PROJECT_DIR)" ]; then \
		echo "lvgl: PROJECT_DIR is empty; cannot locate project include/lv_conf.h" >&2; \
		exit 1; \
	fi
	@if [ ! -f "$(PROJECT_DIR)/include/lv_conf.h" ]; then \
		echo "lvgl: missing $(PROJECT_DIR)/include/lv_conf.h" >&2; \
		exit 1; \
	fi
	@# Do not symlink into $(MODULE_ROOT)/lv_conf.h (absolute links are machine-local
	@# and pollute git). CMake uses PROJECT_PATH/include + LV_CONF_INCLUDE_SIMPLE.
	@rm -f "$(MODULE_ROOT)/lv_conf.h"

	mkdir -p $(BUILD_DIR)
	@export PROJECT_PATH="$(PROJECT_DIR)" && \
	 export PROJECT_DIR="$(PROJECT_DIR)" && \
	 cd $(BUILD_DIR) && cmake $(MODULE_ROOT) \
		-DCONFIG_OS_NAME=linux \
		-DCMAKE_POSITION_INDEPENDENT_CODE=ON \
		-DCMAKE_SYSTEM_NAME=Linux \
		-DCMAKE_C_COMPILER=$(CROSS_CC) \
		-DCMAKE_CXX_COMPILER=$(CROSS_CXX) \
		"-DCMAKE_SHARED_LINKER_FLAGS=$(shell $(CROSS_CC) -print-file-name=libgcc.a)"
	@cd $(BUILD_DIR) && $(MAKE)

install:
	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib
	mkdir -p $(INSTALL_DIR)/$(PROJ)/temp

	cp -f $(BUILD_DIR)/*.a $(INSTALL_DIR)/$(PROJ)/lib
	@# Match zlib/freetype: install shared objects when CMake produced them (Linux).
	@cp -f $(BUILD_DIR)/liblvgl.so* $(INSTALL_DIR)/$(PROJ)/lib/ 2>/dev/null || true

	find $(SRC_DIR)/src -type f -name "*.h" | xargs -i cp -rf --parent {} $(INSTALL_DIR)/$(PROJ)/temp

	rm -rf $(INSTALL_DIR)/$(PROJ)/include
	mkdir -p $(INSTALL_DIR)/$(PROJ)/include

	mv $(INSTALL_DIR)/$(PROJ)/temp$(SRC_DIR)/* $(INSTALL_DIR)/$(PROJ)/include
	rm -rf $(INSTALL_DIR)/$(PROJ)/temp

	cp $(SRC_DIR)/lvgl.h $(INSTALL_DIR)/$(PROJ)/include
	cp $(PROJECT_DIR)/include/lv_conf.h $(INSTALL_DIR)/$(PROJ)/include
	# Copy lv_version.h if it exists (for LVGL 9.3.0 and later versions)
	@if [ -f "$(SRC_DIR)/lv_version.h" ]; then \
		cp $(SRC_DIR)/lv_version.h $(INSTALL_DIR)/$(PROJ)/include; \
	fi
	@if [ -f "$(SRC_DIR)/src/lv_version.h" ]; then \
		cp $(SRC_DIR)/src/lv_version.h $(INSTALL_DIR)/$(PROJ)/include; \
	fi

clean:
	$(RM) -rf $(BUILD_DIR)
	$(RM) -f $(MODULE_ROOT)/lv_conf.h
