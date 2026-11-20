#
# Makefile for anyka 
# Nov. 2024
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)
VERSION := $(CONFIG_THIRD_PARTY_OPENSSL_VERSION)

PROJ = openssl
PROJCRYPTO = crypto

MODULE_ROOT ?= $(shell pwd)
# Use BUILD_DIR passed from parent Makefile, fallback to local build directory
BUILD_DIR ?= $(MODULE_ROOT)/build

SRC_DIR := $(MODULE_ROOT)/$(PROJ)-$(VERSION)

ZLIB_DIR := $(INSTALL_DIR)/zlib


ARCH := $(strip $(CONFIG_THIRD_PARTY_TARGET_ARCH))
ifeq ($(ARCH),)
ARCH = $(findstring arm, $(CROSS_COMPILE))
ifeq ($(ARCH), )
ARCH = $(findstring riscv, $(CROSS_COMPILE))
endif
endif

CFLAGS_STR := $(CFLAGS:-Werror=-Wall)

# standard library must be used
XFLAGS := $(CFLAGS_STR:-nostdlib= )

ifeq ($(XFLAGS), )
XFLAGS := $(CFLAGS)
endif

# Do not pass -static: OpenSSL Configure puts it in LDFLAGS and then disables pic/shared
# (see openssl-3.x Configure around "grep ... -static ... disable('static', 'pic', 'threads')").
# With "shared" below on Linux, both libcrypto/libssl .a and .so are built.
CONFIG_OPTION := --prefix=$(BUILD_DIR) --cross-compile-prefix=$(CROSS_COMPILE) no-asm

# Minimal build for embedded HTTPS (curl): drop legacy/exotic ciphers and unused protocols.
# Typical savings: libcrypto.so ~4MB -> ~1.5-2.5MB (before strip).
ifeq ($(CONFIG_THIRD_PARTY_OPENSSL_MINIMAL),y)
CONFIG_OPTION += no-legacy no-dtls no-engine no-comp no-capieng no-srp no-sctp no-ts \
	no-whirlpool no-idea no-mdc2 no-rc5 no-rc2 no-rc4 no-rmd160 no-md2 no-md4 \
	no-camellia no-seed no-blake2 no-sm2 no-sm3 no-sm4 no-gost no-dgram \
	no-ssl3 no-tls1 no-tls1_1
endif

ifeq ($(CONFIG_OS_NAME), linux)
CONFIG_OPTION += shared
ifeq ($(ARCH), riscv)
CONFIG_OPTION += linux64-riscv64
# There are some assembly code use 'asm' as a keyword, but it's not supported in the c standard while with asm
#XFLAGS += -Dasm=__asm__
else
CONFIG_OPTION += linux-generic32
endif

else
 CONFIG_OPTION += no-shared gcc
endif

CONFIG_OPTION += $(XFLAGS)

##build rule
.PHONY: all build install clean

all: build install
build:
	@# Out-of-tree build: Configure and make in BUILD_DIR
	@# Note: OpenSSL's Configure script doesn't support out-of-tree build well
	@# So we copy the source to BUILD_DIR first
	mkdir -p $(BUILD_DIR)
	@if [ ! -f "$(BUILD_DIR)/Configure" ]; then \
		echo "Copying OpenSSL source to build directory..."; \
		cp -r $(SRC_DIR)/* $(BUILD_DIR)/; \
	fi
	chmod +x $(BUILD_DIR)/Configure $(BUILD_DIR)/config $(BUILD_DIR)/configdata.pm 2>/dev/null || true
	find $(BUILD_DIR)/util -maxdepth 1 -name '*.pl' -o -name '*.sh' 2>/dev/null | xargs chmod +x 2>/dev/null || true
	cd $(BUILD_DIR) && ./Configure $(CONFIG_OPTION)
	cd $(BUILD_DIR) && $(MAKE) -j

install:
	@# Don't run 'make install' as it may fail with install_dev
	@# Instead, copy files directly from BUILD_DIR
	mkdir -p $(INSTALL_DIR)/$(PROJ)/bin
	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib
	mkdir -p $(INSTALL_DIR)/$(PROJ)/include

	@# Copy binaries (apps/openssl)
	@if [ -f "$(BUILD_DIR)/apps/openssl" ]; then \
		cp -f $(BUILD_DIR)/apps/openssl $(INSTALL_DIR)/$(PROJ)/bin/; \
	fi
	
	@# Copy static libraries
	@if [ -f "$(BUILD_DIR)/libssl.a" ]; then \
		cp -f $(BUILD_DIR)/libssl.a $(INSTALL_DIR)/$(PROJ)/lib/; \
	fi
	@if [ -f "$(BUILD_DIR)/libcrypto.a" ]; then \
		cp -f $(BUILD_DIR)/libcrypto.a $(INSTALL_DIR)/$(PROJ)/lib/; \
	fi
	
	@# Copy shared libraries (if exist)
	@if [ -f "$(BUILD_DIR)/libssl.so" ]; then \
		cp -f $(BUILD_DIR)/libssl.so* $(INSTALL_DIR)/$(PROJ)/lib/ 2>/dev/null || true; \
	fi
	@if [ -f "$(BUILD_DIR)/libcrypto.so" ]; then \
		cp -f $(BUILD_DIR)/libcrypto.so* $(INSTALL_DIR)/$(PROJ)/lib/ 2>/dev/null || true; \
	fi
	
	@# Copy headers
	@if [ -d "$(BUILD_DIR)/include/openssl" ]; then \
		mkdir -p $(INSTALL_DIR)/$(PROJ)/include; \
		cp -rf $(BUILD_DIR)/include/openssl $(INSTALL_DIR)/$(PROJ)/include/; \
	fi
	
	@# Create pkgconfig directory structure
	@mkdir -p $(INSTALL_DIR)/$(PROJ)/lib/pkgconfig
	
	@echo "OpenSSL installed to $(INSTALL_DIR)/$(PROJ)"

clean:
	$(RM) -rf $(BUILD_DIR)