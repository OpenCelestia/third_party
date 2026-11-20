#
# Makefile for anyka
# Nov. 2024
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)

PROJ = jpeg
VERSION := $(CONFIG_THIRD_PARTY_JPEG_VERSION)


MODULE_ROOT ?= $(shell pwd)
# Use BUILD_DIR passed from parent Makefile, fallback to local build directory
BUILD_DIR ?= $(MODULE_ROOT)/build

CROSS_TOOLCHAIN := $(shell echo $(CROSS_COMPILE) | sed 's/.$$//')

CONFIG_OPTION := --prefix=$(BUILD_DIR) --host=$(CROSS_TOOLCHAIN)


TARGET = $(INSTALL_DIR)/$(PROJ)/lib


$(shell mkdir -p $(BUILD_DIR))


##build rule
# Parent third_party/Makefile invokes $(MAKE) -C jpeg without a goal name, so the
# first real target becomes the default. Keep `all` first (+ DEFAULT_GOAL).
.PHONY: all target clean stage_to_install
.DEFAULT_GOAL := all

all: vendor_tree
	@# Parent only checks install/.../lib. If that was deleted but BUILD_DIR still has a
	@# successful prefix install (--prefix=$(BUILD_DIR)), just re-copy — do not rebuild.
	@# stage_to_install must use -C MODULE_ROOT: full-build path cds into BUILD_DIR in the
	@# same shell recipe; a bare $(MAKE) would pick up autotools Makefile there.
	@if ls "$(BUILD_DIR)/lib"/libjpeg.so* >/dev/null 2>&1 && [ -d "$(BUILD_DIR)/include" ]; then \
		echo "jpeg: reuse $(BUILD_DIR)/lib -> $(INSTALL_DIR)/$(PROJ) (no reconfigure)"; \
		$(MAKE) -C "$(MODULE_ROOT)" -f Makefile stage_to_install \
			BUILD_DIR="$(BUILD_DIR)" \
			INSTALL_DIR="$(INSTALL_DIR)" \
			PROJ="$(PROJ)" \
			THIRD_PARTY_INSTALL_DIR="$(INSTALL_DIR)" \
			COMPILATION_RULES="$(COMPILATION_RULES)" \
			PROJECT_CONFIG_FILE="$(PROJECT_CONFIG_FILE)"; \
	else \
		echo "jpeg: full build (no usable libs under $(BUILD_DIR)/lib)"; \
		if [ -n "$(WORK_SRC)" ] && [ -d "$(WORK_SRC)" ]; then \
			if [ -f "$(WORK_SRC)/Makefile" ]; then \
				$(MAKE) -C "$(WORK_SRC)" distclean || true; \
			fi; \
			rm -f "$(WORK_SRC)/config.status" "$(WORK_SRC)/config.log" "$(WORK_SRC)/libtool"; \
		fi; \
		rm -rf "$(BUILD_DIR)/autom4te.cache" "$(BUILD_DIR)/.deps" "$(BUILD_DIR)/.libs" 2>/dev/null || true; \
		find "$(BUILD_DIR)" -maxdepth 1 -type f \( -name '*.la' -o -name '*.lo' -o -name '*.o' -o -name '*.a' \) -delete 2>/dev/null || true; \
		rm -f "$(BUILD_DIR)/Makefile" 2>/dev/null || true; \
		rm -f "$(BUILD_DIR)/config.status" "$(BUILD_DIR)/config.log" "$(BUILD_DIR)/libtool" 2>/dev/null || true; \
		mkdir -p $(BUILD_DIR); \
		cd $(BUILD_DIR) && $(WORK_SRC)/configure $(CONFIG_OPTION); \
		cd $(BUILD_DIR) && $(MAKE); \
		cd $(BUILD_DIR) && $(MAKE) install; \
		$(MAKE) -C "$(MODULE_ROOT)" -f Makefile stage_to_install \
			BUILD_DIR="$(BUILD_DIR)" \
			INSTALL_DIR="$(INSTALL_DIR)" \
			PROJ="$(PROJ)" \
			THIRD_PARTY_INSTALL_DIR="$(INSTALL_DIR)" \
			COMPILATION_RULES="$(COMPILATION_RULES)" \
			PROJECT_CONFIG_FILE="$(PROJECT_CONFIG_FILE)"; \
	fi

# Copy already-staged prefix install (BUILD_DIR/{lib,include}) into THIRD_PARTY_INSTALL_DIR.
# Globs must be optional: jpeg may ship only .so or only .a depending on configure.
stage_to_install:
	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib
	@if ls "$(BUILD_DIR)/lib"/*.a >/dev/null 2>&1; then \
		cp -rf "$(BUILD_DIR)/lib"/*.a $(INSTALL_DIR)/$(PROJ)/lib; \
	fi
	@if ls "$(BUILD_DIR)/lib"/libjpeg.so* >/dev/null 2>&1; then \
		cp -rf "$(BUILD_DIR)/lib"/libjpeg.so* $(INSTALL_DIR)/$(PROJ)/lib; \
	fi
	@if [ -d "$(BUILD_DIR)/include" ]; then \
		cp -rf "$(BUILD_DIR)/include" $(INSTALL_DIR)/$(PROJ)/; \
	fi

clean:
	$(RM) -rf $(BUILD_DIR)
