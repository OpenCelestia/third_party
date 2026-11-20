#
# Makefile for zxing — modern zxing-cpp (QR readers) + thin C API.
# Produces libzxing.so / libzxing.a for existing allowlist & zxing_scan_qrcode().
#

include $(COMPILATION_RULES)

INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)
PROJ = zxing

MODULE_ROOT ?= $(shell pwd)
BUILD_DIR ?= $(MODULE_ROOT)/build

# Build the core library only (avoid root CMake pulling docs/wrappers/examples).
ZXING_CPP_SRC := $(MODULE_ROOT)/zxing-cpp/core
ZXING_CPP_BUILD := $(BUILD_DIR)/zxing-cpp
ZXING_CPP_LIB := $(ZXING_CPP_BUILD)/libZXing.a

# Version.h is cmake-generated into ZXING_CPP_BUILD (new in zxing-cpp 3.x;
# BarcodeFormat.h now #includes it). Must run cmake before compiling the wrapper.
INCLUDE := -I$(MODULE_ROOT)/include \
	-I$(MODULE_ROOT)/zxing-cpp/core/src \
	-I$(ZXING_CPP_BUILD)

CXXSRCS := $(MODULE_ROOT)/src/zxing_scan.cpp

OBJS := $(patsubst $(MODULE_ROOT)/%.cpp,$(BUILD_DIR)/%.o,$(CXXSRCS))

STATIC_LIB := lib$(PROJ).a
DYNAMIC_LIB := lib$(PROJ).so

TARGET := $(BUILD_DIR)/$(STATIC_LIB)
ifeq ($(CONFIG_OS_NAME), linux)
TARGET += $(BUILD_DIR)/$(DYNAMIC_LIB)
endif

CXXFLAGS += $(INCLUDE) -std=c++20
CFLAGS += $(INCLUDE)

ifneq (,$(filter aos alios,$(CONFIG_OS_NAME)))
CXXFLAGS += -fno-short-enums -mcpu=arm926ej-s -mfloat-abi=soft -mlong-calls
CXXFLAGS += -fno-common -ffunction-sections -fdata-sections
endif

CXX ?= g++
CMAKE ?= cmake

.PHONY: all target clean install zxing_cpp_lib

all: target install

# Wrapper .o needs generated Version.h from cmake; keep this AFTER `all:` so
# it does not become the default goal.
$(OBJS): $(ZXING_CPP_LIB)

zxing_cpp_lib: $(ZXING_CPP_LIB)

$(ZXING_CPP_LIB):
	@mkdir -p $(ZXING_CPP_BUILD)
	$(CMAKE) -S $(ZXING_CPP_SRC) -B $(ZXING_CPP_BUILD) \
		-DCMAKE_SYSTEM_NAME=Linux \
		-DCMAKE_C_COMPILER="$(CC)" \
		-DCMAKE_CXX_COMPILER="$(CXX)" \
		-DCMAKE_BUILD_TYPE=MinSizeRel \
		-DBUILD_SHARED_LIBS=OFF \
		-DZXING_READERS=ON \
		-DZXING_WRITERS=OFF \
		-DZXING_ENABLE_1D=OFF \
		-DZXING_ENABLE_AZTEC=OFF \
		-DZXING_ENABLE_DATAMATRIX=OFF \
		-DZXING_ENABLE_MAXICODE=OFF \
		-DZXING_ENABLE_PDF417=OFF \
		-DZXING_ENABLE_QRCODE=ON \
		-DCMAKE_CXX_STANDARD=20 \
		-DCMAKE_CXX_STANDARD_REQUIRED=ON
	$(CMAKE) --build $(ZXING_CPP_BUILD) -j$(shell nproc 2>/dev/null || echo 4)

target: $(TARGET)

$(BUILD_DIR)/$(STATIC_LIB): $(OBJS) $(ZXING_CPP_LIB)
	@mkdir -p $(BUILD_DIR)
	rm -f $@
	# Flatten libZXing.a object members into libzxing.a. Nesting the .a as a
	# single member (ar -q libzxing.a libZXing.a) leaves unresolved ZXing::*
	# when consumers link the static archive (framework/tests).
	rm -rf $(BUILD_DIR)/_zxing_ar
	mkdir -p $(BUILD_DIR)/_zxing_ar
	cd $(BUILD_DIR)/_zxing_ar && $(AR) x $(ZXING_CPP_LIB)
	$(AR) -rsv $@ $(OBJS) $(BUILD_DIR)/_zxing_ar/*.o
	$(RANLIB) $@ 2>/dev/null || true
	rm -rf $(BUILD_DIR)/_zxing_ar

ifeq ($(CONFIG_OS_NAME), linux)
$(BUILD_DIR)/$(DYNAMIC_LIB): $(OBJS) $(ZXING_CPP_LIB)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -shared -fPIC -o $@ $(OBJS) \
		-Wl,--whole-archive $(ZXING_CPP_LIB) -Wl,--no-whole-archive \
		-lstdc++ -lm -lpthread $(LDFLAGS)
endif

$(BUILD_DIR)/%.o: $(MODULE_ROOT)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

install: $(TARGET)
	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib
	cp -f $(BUILD_DIR)/$(STATIC_LIB) $(INSTALL_DIR)/$(PROJ)/lib
ifeq ($(CONFIG_OS_NAME), linux)
	cp -f $(BUILD_DIR)/$(DYNAMIC_LIB) $(INSTALL_DIR)/$(PROJ)/lib
endif
	cp -rf $(MODULE_ROOT)/include $(INSTALL_DIR)/$(PROJ)

clean:
	@echo "Cleaning zxing build files..."
	$(RM) -rf $(BUILD_DIR)
