#
# Makefile for anyka 
# Nov. 2023
#

include $(PROJECT_CONFIG_FILE)
include $(COMPILATION_RULES)

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)

PROJ = asr


MODULE_ROOT ?= $(shell pwd)

#$(shell mkdir -p $(BUILD_DIR))
	
##build rule
.PHONY: all clean

all: 
	mkdir -p $(INSTALL_DIR)/$(PROJ)
	cp -rf $(MODULE_ROOT)/wanson/* $(INSTALL_DIR)/$(PROJ)

clean:
	$(RM) $(INSTALL_DIR)/$(PROJ)




