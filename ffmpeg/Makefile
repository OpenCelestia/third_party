#
# Makefile for anyka
# Nov. 2024
#

include $(COMPILATION_RULES)
include $(PROJECT_CONFIG_FILE)
INSTALL_DIR = $(THIRD_PARTY_INSTALL_DIR)

PROJ = ffmpeg
VERSION := $(CONFIG_THIRD_PARTY_FFMPEG_VERSION)

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


PLATFORM_LIBSALL := $(shell find $(SOC_PLATFORM_DIR)/platform/lib -maxdepth 1 -name *.a)
PLATFORM_LIBSALL += $(shell find $(SOC_PLATFORM_DIR)/platform/lib -maxdepth 1 -name *.so)

PLATFORM_LIBS = $(filter-out, libakadpcm.a libakadpcm.so $(PLATFORM_LIBSALL))

# Do not pass -Xlinker -( ... ) via --extra-ldflags: configure link tests run in sh and
# unquoted '(' causes "syntax error: ( unexpected" (worse with --enable-shared).
# Minimal libav* does not need SoC platform archives at configure/make time.
EXTRA_LDFLAGS := -L$(SOC_PLATFORM_DIR)/platform/lib

# Static .a for libmultimedia embed; shared .so for install completeness (not in rootfs allowlist).
CONFIG_OPTION_COMMON := --prefix=$(BUILD_DIR) --enable-cross-compile --cross-prefix=$(CROSS_COMPILE) --arch=$(ARCH) \
	--enable-static --disable-debug --disable-dxva2 --disable-avdevice --disable-swresample --disable-postproc \
	--disable-avfilter --disable-ffplay --disable-swscale --disable-ffprobe --disable-decoder=vvc \
	--disable-programs --disable-doc --enable-pthreads

# Decode-only: call-path SW H.264, plus local audio demux for formats the
# SDK decoder does not take as a raw file (m4a/flac/ogg). No network.
# Size-first: --enable-small, no iconv/zlib, no RVV (keeps call 800x480 decode OK).
ifeq ($(CONFIG_THIRD_PARTY_FFMPEG_DECODE_ONLY),y)
CONFIG_OPTION := $(CONFIG_OPTION_COMMON) --disable-everything \
	--disable-network \
	--disable-iconv --disable-zlib --disable-bzlib --disable-lzma \
	--disable-rvv --enable-small --optflags=-Os \
	--enable-avcodec --enable-avutil --enable-avformat \
	--enable-protocol=file \
	--enable-demuxer=mov,flac,ogg \
	--enable-decoder=h264,flac,vorbis,aac,alac,opus,pcm_s16le \
	--enable-parser=h264,flac,vorbis,aac,opus
# Minimal libav*: align with file_reader.c / video_decoder_sw.c (local + rtsp/rtmp/http).
else ifeq ($(CONFIG_THIRD_PARTY_FFMPEG_MINIMAL),y)
CONFIG_OPTION := $(CONFIG_OPTION_COMMON) --disable-everything \
	--enable-avformat --enable-avcodec --enable-avutil \
	--enable-network \
	--enable-protocol=file,tcp,udp,http,rtsp,rtmp \
	--enable-demuxer=mov,avi,matroska,flv,mpegts,image2,mp3,wav \
	--enable-decoder=h264,hevc,mjpeg,mpeg4,mp3,aac,pcm_s16le,pcm_s16be \
	--enable-parser=h264,hevc,mjpeg,mpeg4,aac,mpegaudio
else
CONFIG_OPTION := $(CONFIG_OPTION_COMMON) --enable-decoder=mpeg4
endif

ifeq ($(VERSION), 4.2.9)
	CONFIG_OPTION += --disable-avresample
endif

# Extra linker flags for configure (library search path only)
EXTRA_CFLAGS := -I$(SOC_PLATFORM_DIR)/platform/include -DCONFIG_PLATFORM_DMA=1 -ffunction-sections -fdata-sections -std=gnu11

ifeq ($(CONFIG_OS_NAME), linux)
	CONFIG_OPTION += --target-os=linux
else
	CONFIG_OPTION += --target-os=none
	EXTRA_LDFLAGS += --specs=nosys.specs -O2 -nostdlib -static -nostartfiles
endif

CONFIG_OPTION += --extra-cflags="$(EXTRA_CFLAGS)"
CONFIG_OPTION += --extra-ldflags="$(EXTRA_LDFLAGS)"

CFLAGS += -Werror

FFMPEG_CONFIG_ENV :=
ifeq ($(ARCH),arm)
# ARM toolchain compatibility: configure-time C11/static_assert probe.
FFMPEG_CONFIG_ENV := CFLAGS="-std=gnu11 -Dstatic_assert=_Static_assert" CPPFLAGS="-Dstatic_assert=_Static_assert"
endif

##build rule
.PHONY: all install clean

all: target install

target: vendor_tree
	@# Out-of-tree build: configure and make in BUILD_DIR (sources under $(WORK_SRC))
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(FFMPEG_CONFIG_ENV) $(WORK_SRC)/configure $(CONFIG_OPTION)
	cd $(BUILD_DIR) && make -j

install:
	cd $(BUILD_DIR) && make install

	mkdir -p $(INSTALL_DIR)/$(PROJ)/lib

	cp -rf  $(BUILD_DIR)/lib/*.a $(INSTALL_DIR)/$(PROJ)/lib
	cp -rf  $(BUILD_DIR)/include $(INSTALL_DIR)/$(PROJ)/

clean:
	$(RM) -rf $(BUILD_DIR)
