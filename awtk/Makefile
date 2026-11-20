#
# Makefile for anyka
# Nov. 2024
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)

PROJ = awtk
VERSION := $(AWTK_VERSION)

MODULE_ROOT ?= $(shell pwd)
SRC_DIR := $(MODULE_ROOT)/$(PROJ)-$(VERSION)
AWTK_ROOT := $(SRC_DIR)
export AWTK_ROOT

# Use BUILD_DIR passed from parent Makefile, fallback to source directory build
# Note: awtk uses scons which typically builds in source tree
BUILD_DIR ?= $(SRC_DIR)/build

CFLAGS += -Werror

##build rule
.PHONY: all clean

all:
	scons

	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib
	mkdir -p $(INSTALL_DIR)/$(PROJ)/include
	mkdir -p $(INSTALL_DIR)/$(PROJ)/temp

ifeq ($(CONFIG_OS_NAME), linux)
	cp -f  $(BUILD_DIR)/bin/*.so $(INSTALL_DIR)/$(PROJ)/lib/
endif

	find $(BUILD_DIR)/var/awtk -type f -name "*.h" | xargs -i cp -rf --parent {} $(INSTALL_DIR)/$(PROJ)/temp
	rm -rf $(INSTALL_DIR)/$(PROJ)/include
	mv $(INSTALL_DIR)/$(PROJ)/temp$(BUILD_DIR)/var/awtk $(INSTALL_DIR)/$(PROJ)/include

	find $(BUILD_DIR)/var/awtk -type f -name "*.o" | xargs -i cp -rf {} $(INSTALL_DIR)/$(PROJ)/temp/

	$(AR) -cr $(INSTALL_DIR)/$(PROJ)/lib/lib$(PROJ).a $(INSTALL_DIR)/$(PROJ)/temp/*.o
	ranlib $(INSTALL_DIR)/$(PROJ)/lib/lib$(PROJ).a

	rm -rf $(INSTALL_DIR)/$(PROJ)/temp

clean:
#	scons clean
	$(RM) -rf $(BUILD_DIR) 
	$(RM) *.pyc



