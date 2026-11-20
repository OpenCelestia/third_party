#
# Makefile for anyka
# Nov. 2024
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)

PROJ = mbedtls
VERSION := $(CONFIG_THIRD_PARTY_MBEDTLS_VERSION)

MODULE_ROOT ?= $(shell pwd)
SRC_DIR := $(MODULE_ROOT)/$(PROJ)-$(VERSION)

# Use BUILD_DIR passed from parent Makefile, fallback to local build directory
BUILD_DIR ?= $(MODULE_ROOT)/build

# Shared .so: Luna curl runtime needs libmbed*.so; Nova/other when CONFIG_THIRD_PARTY_MBEDTLS_SHARED=y
MBEDTLS_BUILD_SHARED := n
ifneq ($(filter y Y,$(CONFIG_THIRD_PARTY_CURL_USE_MBEDTLS)),)
MBEDTLS_BUILD_SHARED := y
endif
ifneq ($(filter y Y,$(CONFIG_THIRD_PARTY_MBEDTLS_SHARED)),)
MBEDTLS_BUILD_SHARED := y
endif

# AliOS/arm-none-eabi is not Unix/Windows for mbedtls platform entropy.
# Project configs use CONFIG_OS_NAME=aos (mira) or =alios (SDK board configs); accept both.
MBEDTLS_IS_AOS := $(if $(filter aos alios,$(CONFIG_OS_NAME)),y)
MBEDTLS_EXTRA_CFLAGS :=
ifneq (,$(MBEDTLS_IS_AOS))
MBEDTLS_EXTRA_CFLAGS += -DMBEDTLS_NO_PLATFORM_ENTROPY
endif

##build rule
.PHONY: all clean

all: vendor_tree
	@if [ -n "$(MBEDTLS_IS_AOS)" ] && [ -f "$(WORK_SRC)/include/mbedtls/config.h" ]; then \
		for _m in MBEDTLS_TIMING_C MBEDTLS_NET_C; do \
			sed -i "s/^#define $${_m}$$/\/\/#define $${_m}/" "$(WORK_SRC)/include/mbedtls/config.h"; \
		done; \
	fi
	@echo "mbedtls: build shared=$(MBEDTLS_BUILD_SHARED) CC=$(CC)"
	@if [ "$(MBEDTLS_BUILD_SHARED)" = "y" ]; then \
		$(MAKE) -C "$(WORK_SRC)/library" CC="$(CC)" AR="$(AR)" CFLAGS="$(CFLAGS) $(MBEDTLS_EXTRA_CFLAGS)" SHARED=1; \
	else \
		$(MAKE) -C "$(WORK_SRC)/library" CC="$(CC)" AR="$(AR)" CFLAGS="$(CFLAGS) $(MBEDTLS_EXTRA_CFLAGS)" static; \
	fi
	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib
	mkdir -p $(INSTALL_DIR)/$(PROJ)/include
	cp -af $(WORK_SRC)/library/libmbed*.a $(INSTALL_DIR)/$(PROJ)/lib/
	if [ "$(MBEDTLS_BUILD_SHARED)" = "y" ]; then \
		cp -af $(WORK_SRC)/library/libmbed*.so* $(INSTALL_DIR)/$(PROJ)/lib/; \
		if ! ls $(INSTALL_DIR)/$(PROJ)/lib/libmbedtls.so* >/dev/null 2>&1; then \
			echo "Error: mbedtls shared libs missing in $(INSTALL_DIR)/$(PROJ)/lib"; \
			echo "  build dir: $(WORK_SRC)/library"; \
			echo "  Hint: rm -rf $(BUILD_DIR) $(INSTALL_DIR)/$(PROJ) && rebuild third_party/mbedtls"; \
			exit 1; \
		fi; \
		echo "mbedtls shared: $$(ls $(INSTALL_DIR)/$(PROJ)/lib/libmbed*.so* | tr '\n' ' ')"; \
	fi
	cp -af $(WORK_SRC)/include/mbedtls $(INSTALL_DIR)/$(PROJ)/include/

clean:
	$(RM) -rf $(BUILD_DIR)



