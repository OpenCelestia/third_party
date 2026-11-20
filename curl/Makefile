#
# Makefile for anyka 
# Nov. 2024
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)

PROJ = curl
VERSION := $(CONFIG_THIRD_PARTY_CURL_VERSION)

MODULE_ROOT ?= $(shell pwd)

SRC_DIR := $(MODULE_ROOT)/$(PROJ)-$(VERSION)

# Use BUILD_DIR passed from parent Makefile, fallback to local build directory
BUILD_DIR ?= $(MODULE_ROOT)/build

#remove the lastest chat 
CROSS_TOOLCHAIN := $(shell echo $(CROSS_COMPILE) | sed 's/.$$//')

ZLIB_INSTALL_DIR := $(THIRD_PARTY_INSTALL_DIR)/zlib
OPENSSL_INSTALL_DIR := $(THIRD_PARTY_INSTALL_DIR)/openssl
MBEDTLS_INSTALL_DIR := $(THIRD_PARTY_INSTALL_DIR)/mbedtls

# Luna (volc off): curl HTTPS via mbedtls — no libcrypto.so on rootfs.
ifeq ($(CONFIG_THIRD_PARTY_CURL_USE_MBEDTLS),y)
CFLAGS := -I$(MBEDTLS_INSTALL_DIR)/include
LDFLAGS := -L$(MBEDTLS_INSTALL_DIR)/lib -L$(ZLIB_INSTALL_DIR)/lib \
	-Wl,-rpath-link=$(MBEDTLS_INSTALL_DIR)/lib -Wl,-rpath-link=$(ZLIB_INSTALL_DIR)/lib
CURL_SSL_CONFIG := --with-mbedtls=$(MBEDTLS_INSTALL_DIR) --without-openssl
else
CFLAGS := -I$(OPENSSL_INSTALL_DIR)/include
# libcurl.so lists DT_NEEDED libssl.so.3 / libcrypto.so.3; linking the curl binary must
# resolve those deps. -L alone is often insufficient; -rpath-link fixes "not found" + undefined OPENSSL_*.
LDFLAGS := -L$(OPENSSL_INSTALL_DIR)/lib -L$(ZLIB_INSTALL_DIR)/lib \
	-Wl,-rpath-link=$(OPENSSL_INSTALL_DIR)/lib -Wl,-rpath-link=$(ZLIB_INSTALL_DIR)/lib
CURL_SSL_CONFIG := --with-openssl=$(OPENSSL_INSTALL_DIR) --without-mbedtls
endif

LIBS := 

PKG_CONFIG ?= $(shell command -v $(CROSS_TOOLCHAIN)-pkg-config 2>/dev/null || command -v pkg-config 2>/dev/null)
SYSTOOT=/opt/$(CROSS_TOOLCHAIN)/$(CROSS_TOOLCHAIN)/sysroot
USR_PATH=$(SYSTOOT)/usr

CONFIG_OPTION := --prefix=$(BUILD_DIR) --host=$(CROSS_TOOLCHAIN) CC=$(CROSS_TOOLCHAIN)-gcc CXX=$(CROSS_TOOLCHAIN)-g++ \
				 $(CURL_SSL_CONFIG) --without-nss --with-zlib --without-libpsl --without-brotli --without-zstd \
				 --without-nghttp2 --without-ngtcp2 --without-quiche --without-msh3 \
				 --disable-ldap --disable-ldaps --disable-rtsp --disable-dict --disable-telnet --disable-tftp \
				 --disable-pop3 --disable-imap --disable-smb --disable-smtp --disable-gopher \
				 --disable-manual PKG_CONFIG="$(PKG_CONFIG)" --with-sysroot=$(SYSTOOT)

ifeq ($(CONFIG_OS_NAME), linux)

else
	LIBS += $(shell find $(PLATFORM_PATH)/ -maxdepth 10 -name *.a)
	LDFLAGS += "--specs=nosys.specs -O2 -nostdlib -static -nostartfiles"
	CONFIG_OPTION += --disable-shared
endif

CONFIG_OPTION += CPPFLAGS=$(CFLAGS) LDFLAGS="$(LDFLAGS)" LIBS=$(LIBS)

CFLAGS += -Werror


$(shell mkdir -p $(BUILD_DIR))


##build rule
.PHONY: all target clean

all:
	@# curl's out-of-tree build has issues, so copy source to BUILD_DIR
	@# (similar to OpenSSL approach)
	@if [ "$(CONFIG_THIRD_PARTY_CURL_USE_MBEDTLS)" = "y" ]; then \
		NEED_MBEDTLS=0; \
		if [ ! -f "$(MBEDTLS_INSTALL_DIR)/include/mbedtls/ssl.h" ]; then NEED_MBEDTLS=1; fi; \
		if ! ls "$(MBEDTLS_INSTALL_DIR)/lib"/libmbedtls.so* >/dev/null 2>&1; then NEED_MBEDTLS=1; fi; \
		if [ "$$NEED_MBEDTLS" = "1" ]; then \
			echo "mbedtls headers or shared libs missing, building mbedtls dependency first..."; \
			MBEDTLS_BUILD_DIR="$(patsubst %/curl/,%/mbedtls/,$(patsubst %/curl,%/mbedtls,$(BUILD_DIR)))"; \
			$(MAKE) -C $(MODULE_ROOT)/../mbedtls \
				-f Makefile \
				-f ../vendor_rules.mk \
				BUILD_DIR="$$MBEDTLS_BUILD_DIR" \
				THIRD_PARTY_INSTALL_DIR=$(THIRD_PARTY_INSTALL_DIR) \
				PROJECT_DIR=$(PROJECT_DIR) \
				COMPILATION_RULES=$(COMPILATION_RULES) \
				PROJECT_CONFIG_FILE=$(PROJECT_CONFIG_FILE) || exit 1; \
		fi; \
	fi
	@# Vendor ships pre-generated configure/Makefile.in from the release tarball; do not autoreconf on the build host.
	@for f in configure config.guess config.sub missing compile install-sh aclocal.m4 ltmain.sh Makefile.in lib/curl_config.h.in; do \
		if [ ! -f "$(SRC_DIR)/$$f" ]; then \
			echo "ERROR: $(SRC_DIR)/$$f missing — sync third_party/$(PROJ) from git (curl release autotools)"; \
			exit 1; \
		fi; \
	done
	mkdir -p $(BUILD_DIR)
	@# Re-stage when BUILD_DIR is empty or left over from a failed autoreconf (configure without aux files).
	@NEED_STAGE=0; \
	for f in configure config.guess config.sub missing compile install-sh lib/curl_config.h.in; do \
		if [ ! -f "$(BUILD_DIR)/$$f" ]; then NEED_STAGE=1; break; fi; \
	done; \
	if [ "$$NEED_STAGE" = "1" ]; then \
		echo "Copying curl source to build directory..."; \
		rm -rf "$(BUILD_DIR)"; \
		mkdir -p "$(BUILD_DIR)"; \
		cp -a "$(SRC_DIR)/." "$(BUILD_DIR)/"; \
		find "$(BUILD_DIR)" -type f \( -name '*.sh' -o -name buildconf -o -name configure -o -name config.guess -o -name config.sub -o -name install-sh -o -name compile \) -exec chmod +x {} + 2>/dev/null || true; \
	fi
	@for f in configure config.guess config.sub missing compile install-sh lib/curl_config.h.in; do \
		if [ ! -f "$(BUILD_DIR)/$$f" ]; then \
			echo "ERROR: $(BUILD_DIR)/$$f still missing after staging"; \
			exit 1; \
		fi; \
	done
	cd $(BUILD_DIR) && ./configure $(CONFIG_OPTION)
	cd $(BUILD_DIR) && make -j

	@# Don't use 'make install' - copy files directly (like OpenSSL)
	@# This avoids install-pkgincludeHEADERS errors
	mkdir -p $(INSTALL_DIR)/$(PROJ)/bin
	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib
	mkdir -p $(INSTALL_DIR)/$(PROJ)/include

	@# Copy binaries
	@if [ -d "$(BUILD_DIR)/src/.libs" ]; then \
		cp -f $(BUILD_DIR)/src/.libs/curl $(INSTALL_DIR)/$(PROJ)/bin/ 2>/dev/null || true; \
	fi
	@if [ -f "$(BUILD_DIR)/src/curl" ]; then \
		cp -f $(BUILD_DIR)/src/curl $(INSTALL_DIR)/$(PROJ)/bin/ 2>/dev/null || true; \
	fi

	@# Copy static libraries
	@if [ -d "$(BUILD_DIR)/lib/.libs" ]; then \
		cp -f $(BUILD_DIR)/lib/.libs/libcurl.a $(INSTALL_DIR)/$(PROJ)/lib/ 2>/dev/null || true; \
	fi
	@if [ -f "$(BUILD_DIR)/lib/.libs/libcurl.so" ]; then \
		cp -f $(BUILD_DIR)/lib/.libs/libcurl.so* $(INSTALL_DIR)/$(PROJ)/lib/ 2>/dev/null || true; \
	fi

	@# Copy headers
	@if [ -d "$(BUILD_DIR)/include/curl" ]; then \
		cp -rf $(BUILD_DIR)/include/curl $(INSTALL_DIR)/$(PROJ)/include/; \
	fi
	@if [ -d "$(SRC_DIR)/include/curl" ] && [ ! -d "$(INSTALL_DIR)/$(PROJ)/include/curl" ]; then \
		cp -rf $(SRC_DIR)/include/curl $(INSTALL_DIR)/$(PROJ)/include/; \
	fi

	@echo "curl installed to $(INSTALL_DIR)/$(PROJ)"

clean:
	$(RM) -rf $(BUILD_DIR)