#
# Makefile for anyka
# Nov. 2024
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)

PROJ = tslib
VERSION := $(CONFIG_THIRD_PARTY_TSLIB_VERSION)

MODULE_ROOT ?= $(shell pwd)
# Use BUILD_DIR passed from parent Makefile, fallback to local build directory
BUILD_DIR ?= $(MODULE_ROOT)/build

#remove the lastest chat
CROSS_TOOLCHAIN := $(shell echo $(CROSS_COMPILE) | sed 's/.$$//')

CONFIG_OPTION := --host=$(CROSS_TOOLCHAIN) CC=$(CC) --prefix=$(BUILD_DIR) --cache-file=tmp.cache --enable-static=yes \
	--enable-input=yes --enable-linear=yes --enable-invert=yes --enable-dejitter=yes --enable-pthres=yes

ifeq ($(CONFIG_OS_NAME), linux)
	CONFIG_OPTION += --enable-shared=yes
else
	CONFIG_OPTION += --extra-ldflags="--specs=nosys.specs -O2 -nostdlib -static -nostartfiles"
endif

CFLAGS += -Werror

##build rule
.PHONY: all clean

all: vendor_tree
	@# Vendor ships pre-generated configure/aclocal; do not regenerate on the build host.
	@if [ ! -f "$(VENDOR_SRC)/configure" ]; then \
		echo "ERROR: $(VENDOR_SRC)/configure missing — sync third_party/$(PROJ) from git"; \
		exit 1; \
	fi
	@if [ ! -f "$(WORK_SRC)/configure" ]; then \
		echo "third_party/$(PROJ): $(WORK_SRC)/configure missing, re-staging vendor tree"; \
		rm -rf "$(WORK_SRC)"; \
		mkdir -p "$(WORK_SRC)" && cp -a "$(VENDOR_SRC)/." "$(WORK_SRC)/"; \
		touch "$(WORK_SRC)/.vendor_ready"; \
		find "$(WORK_SRC)" -type f \( -name '*.sh' -o -name configure -o -name config.guess -o -name config.sub -o -name install-sh -o -name compile \) -exec chmod +x {} + 2>/dev/null || true; \
	fi
	@rm -f "$(WORK_SRC)/autogen.sh" 2>/dev/null || true
	@if [ ! -f "$(WORK_SRC)/configure" ]; then \
		echo "ERROR: $(WORK_SRC)/configure still missing after re-stage"; \
		exit 1; \
	fi
	@# Do not use distclean: it can drop autotools inputs and break the next configure.
	@if [ -f "$(WORK_SRC)/Makefile" ]; then \
		$(MAKE) -C "$(WORK_SRC)" clean 2>/dev/null || true; \
	fi
	@rm -f "$(WORK_SRC)/config.status" "$(WORK_SRC)/config.log" "$(WORK_SRC)/libtool" 2>/dev/null || true

	@# Build in $(WORK_SRC): vendor copy is already under out/; avoids VPATH aclocal regen.
	mkdir -p $(BUILD_DIR)
	echo "ac_cv_func_malloc_0_nonnull=yes" > $(WORK_SRC)/tmp.cache
	cd $(WORK_SRC) && ./configure $(CONFIG_OPTION) --cache-file=tmp.cache
	@# Do not touch configure: make would rerun config.status --recheck and autotools' clock check fails.
	@touch $(WORK_SRC)/aclocal.m4 $(WORK_SRC)/Makefile.in $(WORK_SRC)/config.h.in 2>/dev/null || true
	cd $(WORK_SRC) && make
	cd $(WORK_SRC) && make install

	@# Install only runtime artifacts (lib/include), not build tree (.o, Makefile, config.status, ...).
	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib $(INSTALL_DIR)/$(PROJ)/include
	cp -af $(BUILD_DIR)/lib/libts.so* $(INSTALL_DIR)/$(PROJ)/lib/ 2>/dev/null || true
	cp -af $(BUILD_DIR)/lib/libts.a $(INSTALL_DIR)/$(PROJ)/lib/ 2>/dev/null || true
	if [ -d "$(BUILD_DIR)/lib/ts" ]; then \
		cp -af $(BUILD_DIR)/lib/ts $(INSTALL_DIR)/$(PROJ)/lib/; \
	fi
	cp -af $(BUILD_DIR)/include $(INSTALL_DIR)/$(PROJ)/ 2>/dev/null || true
	if ! ls $(INSTALL_DIR)/$(PROJ)/lib/libts.so* $(INSTALL_DIR)/$(PROJ)/lib/libts.a >/dev/null 2>&1; then \
		echo "Error: tslib install missing libts in $(INSTALL_DIR)/$(PROJ)/lib"; \
		exit 1; \
	fi
	echo "tslib installed: $$(ls $(INSTALL_DIR)/$(PROJ)/lib/libts* 2>/dev/null | tr '\n' ' ')"
clean:
	$(RM) -rf $(BUILD_DIR)

