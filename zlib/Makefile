#
# Makefile for anyka
# Nov. 2024
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)

PROJ = zlib
VERSION := $(CONFIG_THIRD_PARTY_ZLIB_VERSION)

MODULE_ROOT ?= $(shell pwd)

# Use BUILD_DIR passed from parent Makefile, fallback to local build directory
BUILD_DIR ?= $(MODULE_ROOT)/build

ARCH := $(strip $(CONFIG_THIRD_PARTY_TARGET_ARCH))
ifeq ($(ARCH),)
ARCH = $(findstring arm, $(CROSS_COMPILE))
ifeq ($(ARCH), )
ARCH = $(findstring riscv, $(CROSS_COMPILE))
endif
endif




SOC_PLATFORM_LIBS := $(shell find $(SOC_PLATFORM_DIR)/platform/lib -maxdepth 1 -name *.a)
SOC_PLATFORM_LIBS += $(shell find $(SOC_PLATFORM_DIR)/platform/lib -maxdepth 1 -name *.so)
LDFLAGS := -Xlinker -( $(SOC_PLATFORM_LIBS) -Xlinker -)

export CC := $(CC)
export CXX := $(CXX)
export AD := $(AS)
export LD := $(LD)


# CONFIG_OPTION := --prefix=$(BUILD_DIR) --arch=$(ARCH) --static
CONFIG_OPTION := --prefix=$(BUILD_DIR)

#CONFIG_OPTION += --extra-cflags="-I$(SOC_PLATFORM_DIR)/platform/include " --extra-ldflags="-L$(PLATFORM_PATH)/platform/lib $(LDFLAGS)"


CFLAGS += -Werror

##build rule
.PHONY: all install clean

all: target install

target: vendor_tree
	@# Out-of-tree build: configure and make in BUILD_DIR (sources only under $(WORK_SRC))
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(WORK_SRC)/configure $(CONFIG_OPTION)
	cd $(BUILD_DIR) && make -j

install:
	cd $(BUILD_DIR) && make install

	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib

	cp -rf  $(BUILD_DIR)/lib/*.a $(INSTALL_DIR)/$(PROJ)/lib
	cp -rf  $(BUILD_DIR)/lib/*.so* $(INSTALL_DIR)/$(PROJ)/lib
	cp -rf  $(BUILD_DIR)/include $(INSTALL_DIR)/$(PROJ)/

clean:
	$(RM) -rf $(BUILD_DIR)
