#
# Makefile for anyka
# Nov. 2024
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)

PROJ = freetype
VERSION := $(CONFIG_THIRD_PARTY_FREETYPE_VERSION)

MODULE_ROOT ?= $(shell pwd)
# Use BUILD_DIR passed from parent Makefile, fallback to local build directory
BUILD_DIR ?= $(MODULE_ROOT)/build

TARGET := $(INSTALL_DIR)/$(PROJ)/lib

ZLIB_INSTALL_DIR := $(THIRD_PARTY_INSTALL_DIR)/zlib

CFLAGS += -Werror

ARCH := $(strip $(CONFIG_THIRD_PARTY_TARGET_ARCH))
ifeq ($(ARCH),)
ARCH = $(findstring arm, $(CROSS_COMPILE))
ifeq ($(ARCH), )
ARCH = $(findstring riscv, $(CROSS_COMPILE))
endif
endif
ifeq ($(ARCH),)
$(error third_party/freetype: cannot determine ARCH, set CONFIG_THIRD_PARTY_TARGET_ARCH in project config.mk)
endif

CONFIG_OPTION := CC=$(CC) --prefix=$(BUILD_DIR) --enable-static --with-png=no \
	--without-bzip2 --without-harfbuzz --without-brotli

# Luna/LVGL only needs TrueType (.ttf). Minimal modules drop ~40-60% vs full freetype.
ifeq ($(CONFIG_THIRD_PARTY_FREETYPE_MINIMAL),y)
CONFIG_OPTION += --with-old-mac-fonts=no
endif

ifeq ($(CONFIG_THIRD_PARTY_ZLIB), y)
ifeq ($(CONFIG_THIRD_PARTY_FREETYPE_MINIMAL),y)
# LVGL TTF-only: no WOFF/.gz fonts; sfnt must not call FT_Gzip_Uncompress without gzip aux module.
CONFIG_OPTION += --with-zlib=no
else
CONFIG_OPTION += --with-zlib=yes ZLIB_CFLAGS="-I$(ZLIB_INSTALL_DIR)/include" ZLIB_LIBS="-L$(ZLIB_INSTALL_DIR)/lib -lz"
endif
else
CONFIG_OPTION += --with-zlib=no
endif

ifeq ($(CONFIG_OS_NAME), linux)
ifeq ($(CONFIG_THIRD_PARTY_FREETYPE_SHARED),y)
	CONFIG_OPTION += --host=$(ARCH)-linux --enable-shared
else
	CONFIG_OPTION += --host=$(ARCH)-linux --disable-shared
endif
else ifeq ($(CONFIG_OS_NAME), rtt)
	CONFIG_OPTION += --host=$(ARCH)-none CFLAGS="-O2 -std=c99 -mlittle-endian -fno-short-enums -mfloat-abi=soft -mno-unaligned-access" LDFLAGS="--specs=nosys.specs -O2"
else ifeq ($(CONFIG_OS_NAME), alios)
	CONFIG_OPTION += --host=$(ARCH)-none LDFLAGS="--specs=nosys.specs -O2 -nostdlib -static -nostartfiles"
endif



.PHONY: all  clean freetype_build

all: freetype_build

freetype_build: vendor_tree
ifeq ($(CONFIG_THIRD_PARTY_ZLIB), y)
	@if [ ! -f "$(ZLIB_INSTALL_DIR)/include/zlib.h" ]; then \
		echo "zlib headers not found, building zlib dependency first..."; \
		ZLIB_BUILD_DIR="$(patsubst %/freetype/,%/zlib/,$(patsubst %/freetype,%/zlib,$(BUILD_DIR)))"; \
		$(MAKE) -C $(MODULE_ROOT)/../zlib \
			-f Makefile \
			-f ../vendor_rules.mk \
			BUILD_DIR="$$ZLIB_BUILD_DIR" \
			THIRD_PARTY_INSTALL_DIR=$(THIRD_PARTY_INSTALL_DIR) \
			PROJECT_DIR=$(PROJECT_DIR) \
			COMPILATION_RULES=$(COMPILATION_RULES) \
			PROJECT_CONFIG_FILE=$(PROJECT_CONFIG_FILE) || exit 1; \
	fi; \
	if [ ! -f "$(ZLIB_INSTALL_DIR)/include/zlib.h" ]; then \
		echo "Error: zlib headers still missing: $(ZLIB_INSTALL_DIR)/include/zlib.h"; \
		exit 1; \
	fi
endif
	@# LVGL TTF-only: trim font drivers in modules.cfg and disable zlib/WOFF consistently.
	@# FreeType configure uses builds/unix/ftoption.h as template; patch both copies,
	@# otherwise sfwoff.c may call FT_Gzip_Uncompress while gzip module has been removed.
	@if [ "$(CONFIG_THIRD_PARTY_FREETYPE_MINIMAL)" = "y" ]; then \
		MCFG="$(WORK_SRC)/modules.cfg"; \
		if [ -f "$$MCFG" ]; then \
			sed -i \
				-e 's/^FONT_MODULES += type1/# FONT_MODULES += type1/' \
				-e 's/^FONT_MODULES += cff/# FONT_MODULES += cff/' \
				-e 's/^FONT_MODULES += cid/# FONT_MODULES += cid/' \
				-e 's/^FONT_MODULES += pfr/# FONT_MODULES += pfr/' \
				-e 's/^FONT_MODULES += type42/# FONT_MODULES += type42/' \
				-e 's/^FONT_MODULES += winfonts/# FONT_MODULES += winfonts/' \
				-e 's/^FONT_MODULES += pcf/# FONT_MODULES += pcf/' \
				-e 's/^FONT_MODULES += bdf/# FONT_MODULES += bdf/' \
				-e 's/^RASTER_MODULES += svg/# RASTER_MODULES += svg/' \
				-e 's/^RASTER_MODULES += sdf/# RASTER_MODULES += sdf/' \
				-e 's/^AUX_MODULES += cache/# AUX_MODULES += cache/' \
				-e 's/^AUX_MODULES += gzip/# AUX_MODULES += gzip/' \
				-e 's/^AUX_MODULES += lzw/# AUX_MODULES += lzw/' \
				-e 's/^AUX_MODULES += bzip2/# AUX_MODULES += bzip2/' \
				-e 's/^AUX_MODULES += psaux/# AUX_MODULES += psaux/' \
				-e 's/^AUX_MODULES += psnames/# AUX_MODULES += psnames/' \
				"$$MCFG"; \
		fi; \
		for FOPT in \
			"$(WORK_SRC)/include/freetype/config/ftoption.h" \
			"$(WORK_SRC)/builds/unix/ftoption.h"; do \
			if [ -f "$$FOPT" ]; then \
				sed -i \
					-e 's/^#define FT_CONFIG_OPTION_USE_ZLIB/\/\* & \*\//' \
					-e 's/^#define FT_CONFIG_OPTION_USE_LZW/\/\* & \*\//' \
					"$$FOPT"; \
			fi; \
		done; \
	fi
	@# Out-of-tree build: configure and make in BUILD_DIR
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(WORK_SRC)/configure $(CONFIG_OPTION)
	@if [ "$(CONFIG_THIRD_PARTY_FREETYPE_MINIMAL)" = "y" ]; then \
		for FOPT in \
			"$(BUILD_DIR)/include/freetype2/freetype/config/ftoption.h" \
			"$(BUILD_DIR)/include/freetype/config/ftoption.h" \
			"$(BUILD_DIR)/ftoption.h"; do \
			if [ -f "$$FOPT" ]; then \
				sed -i \
					-e 's/^#define FT_CONFIG_OPTION_USE_ZLIB/\/\* & \*\//' \
					-e 's/^#define FT_CONFIG_OPTION_USE_LZW/\/\* & \*\//' \
					"$$FOPT"; \
			fi; \
		done; \
	fi
	cd $(BUILD_DIR) && make -j
	
	cd $(BUILD_DIR) && make install
	@if [ ! -d "$(BUILD_DIR)/include" ]; then \
		echo "Error: freetype install output missing include directory: $(BUILD_DIR)/include"; \
		exit 1; \
	fi
	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib
	cp -rf  $(BUILD_DIR)/lib/*.a $(INSTALL_DIR)/$(PROJ)/lib
ifeq ($(CONFIG_OS_NAME), linux)
ifeq ($(CONFIG_THIRD_PARTY_FREETYPE_SHARED),y)
	cp -rf  $(BUILD_DIR)/lib/*.so* $(INSTALL_DIR)/$(PROJ)/lib
endif
endif
	cp -rf  $(BUILD_DIR)/include $(INSTALL_DIR)/$(PROJ)/

clean:
	$(RM) -rf $(BUILD_DIR)
	@FT="$(MODULE_ROOT)/$(PROJ)-$(VERSION)"; \
	if [ ! -d "$$FT" ]; then exit 0; fi; \
	rm -f "$$FT/builds/unix/config.log" "$$FT/builds/unix/config.status" "$$FT/builds/unix/libtool" \
		"$$FT/builds/unix/unix-def.mk" "$$FT/builds/unix/unix-cc.mk" "$$FT/builds/unix/ftconfig.h" 2>/dev/null || true; \
	rm -rf "$$FT/objs/.libs" 2>/dev/null || true; \
	rm -f "$$FT/objs/libfreetype.la" "$$FT/objs/ftmodule.h" 2>/dev/null || true

