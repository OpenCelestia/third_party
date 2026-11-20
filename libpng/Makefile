#
# Makefile for anyka
# Nov. 2024
#
# Modified to use BUILD_DIR from parent Makefile
# All .o files will be generated in BUILD_DIR, not in source directory

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)

PROJ = libpng
VERSION := $(CONFIG_THIRD_PARTY_LIBPNG_VERSION)

MODULE_ROOT ?= $(shell pwd)

# Use BUILD_DIR passed from parent Makefile, fallback to local build directory
BUILD_DIR ?= $(MODULE_ROOT)/build

SRC_DIR := $(MODULE_ROOT)/$(PROJ)-$(VERSION)
ZLIB_INSTALL_DIR := $(THIRD_PARTY_INSTALL_DIR)/zlib

TARGET :=

STATIC_LIB := $(PROJ).a
TARGET += $(BUILD_DIR)/$(STATIC_LIB)

# Find all source files, exclude examples and test files
SRCS := $(shell find $(SRC_DIR) ! -path "*examples*" -name "*.c" \
	-not -name "*example.c" \
	-not -name "pngtest.c" \
	-not -name "*test*.c")

# Generate object files in BUILD_DIR, preserving directory structure
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

CFLAGS += -O2 -w -Wno-implicit-function-declaration -fPIC

INCLUDE += -I$(PLATFORM_PATH)/platform/include
INCLUDE += -I$(PROJECT_PATH)/include

INCLUDE += -I$(SRC_DIR)
INCLUDE += -I$(ZLIB_INSTALL_DIR)/include

# Create BUILD_DIR and subdirectories
$(shell mkdir -p $(BUILD_DIR))
$(foreach obj,$(OBJS),$(shell mkdir -p $(dir $(obj))))

# Real SONAME libpng16.so (musl dlopen / FAT SD: no dangling symlink-only names).
SHARED_LIB := libpng16.so
SHARED_LIB_ALIAS := libpng.so
TARGET_SHARED := $(BUILD_DIR)/$(SHARED_LIB)

##build rule
.PHONY: all install clean

all: target install

target: $(TARGET) $(TARGET_SHARED)
	@if [ ! -d $(SRC_DIR) ] ; then tar xvf $(SRC_DIR).tar.* ; fi
ifeq ($(CONFIG_THIRD_PARTY_ZLIB), y)
	@if [ ! -f "$(ZLIB_INSTALL_DIR)/include/zlib.h" ]; then \
		echo "zlib headers not found, building zlib dependency first..."; \
		ZLIB_BUILD_DIR="$(patsubst %/libpng/,%/zlib/,$(patsubst %/libpng,%/zlib,$(BUILD_DIR)))"; \
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

# Compilation rule: compile .c files from SRC_DIR to .o files in BUILD_DIR
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $< -> $@"
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# Link all object files into static library
$(TARGET): $(OBJS)
	@echo "Creating static library: $@"
	@echo "  Using $(words $(OBJS)) object files"
	$(AR) -rsv $@ $(OBJS)
	@echo "Library created successfully: $@"

# Shared for Scheme B deferred dlopen (Home does not need PNG at first frame).
# Use -soname libpng16.so; install hard copies of both names (SD may be FAT, no symlinks).
$(TARGET_SHARED): $(OBJS)
	@echo "Creating shared library: $@"
	$(CC) -shared -fPIC -Wl,-soname,$(SHARED_LIB) -o $@ $(OBJS) -L$(ZLIB_INSTALL_DIR)/lib -lz
	@cp -f $@ $(BUILD_DIR)/$(SHARED_LIB_ALIAS)
	@echo "Shared library created successfully: $@ (+ $(SHARED_LIB_ALIAS))"

install:
	@echo "Installing libpng to $(INSTALL_DIR)/$(PROJ)"
	@mkdir -p $(INSTALL_DIR)/$(PROJ)/lib
	@mkdir -p $(INSTALL_DIR)/$(PROJ)/temp
	
	@echo "  Copying library files..."
	@cp $(BUILD_DIR)/*.a $(INSTALL_DIR)/$(PROJ)/lib
	@if [ ! -f "$(BUILD_DIR)/$(SHARED_LIB)" ]; then \
		echo "Error: $(BUILD_DIR)/$(SHARED_LIB) missing"; exit 1; \
	fi
	@cp -f $(BUILD_DIR)/$(SHARED_LIB) $(INSTALL_DIR)/$(PROJ)/lib/$(SHARED_LIB)
	@cp -f $(BUILD_DIR)/$(SHARED_LIB) $(INSTALL_DIR)/$(PROJ)/lib/$(SHARED_LIB_ALIAS)
	
	@echo "  Copying header files..."
	@find $(SRC_DIR) -type f -name "*.h" | xargs -i cp -rf --parent {} $(INSTALL_DIR)/$(PROJ)/temp
	
	@rm -rf $(INSTALL_DIR)/$(PROJ)/include
	@mkdir -p $(INSTALL_DIR)/$(PROJ)/include
	
	@mv $(INSTALL_DIR)/$(PROJ)/temp$(SRC_DIR)/* $(INSTALL_DIR)/$(PROJ)/include
	
	@rm -rf $(INSTALL_DIR)/$(PROJ)/temp
	
	@echo "  Verifying installation..."
	@ls -lh $(INSTALL_DIR)/$(PROJ)/lib/
	@if [ -f "$(INSTALL_DIR)/$(PROJ)/lib/libpng.a" ]; then \
		echo "  ✓ libpng.a installed successfully"; \
	fi
	@if [ -f "$(INSTALL_DIR)/$(PROJ)/lib/$(SHARED_LIB)" ] && [ -f "$(INSTALL_DIR)/$(PROJ)/lib/$(SHARED_LIB_ALIAS)" ]; then \
		echo "  ✓ $(SHARED_LIB) + $(SHARED_LIB_ALIAS) installed (hard copies)"; \
	else \
		echo "Error: libpng shared libs incomplete under $(INSTALL_DIR)/$(PROJ)/lib"; exit 1; \
	fi

clean:
	@echo "Cleaning libpng build files..."
	@echo "  Removing object files in BUILD_DIR: $(BUILD_DIR)"
	$(RM) -rf $(BUILD_DIR)
	@echo "  Clean complete"