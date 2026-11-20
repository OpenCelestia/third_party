#
# Makefile for anyka airos third party
# May 2025
#
# 本 Makefile 负责编译 third_party 目录下的所有第三方库子模块
# 
# 关键变量（由 build.sh 传递）：
#   - BUILD_THIRD_PARTY_DIR: 编译中间产物存放目录，例如 out/build/third_party
#   - SRC_THIRD_PARTY_DIR: 源码目录，例如 third_party/
#   - THIRD_PARTY_INSTALL_DIR: 安装目录，例如 out/install/third_party
#
include $(PROJECT_CONFIG_FILE)
include $(COMPILATION_RULES)


obj-y =
OBJS_CPP-y =
HEADDIR-y =
LIBS-y =
LIBS-Static-y =

obj-$(CONFIG_THIRD_PARTY_CJSON)				+= cjson/
obj-$(CONFIG_THIRD_PARTY_INIPARSER)			+= iniparser/
obj-$(CONFIG_THIRD_PARTY_ZXING)				+= zxing/
obj-$(CONFIG_THIRD_PARTY_ASRIB)				+= asr/
obj-$(CONFIG_THIRD_PARTY_ZLIB) 				+= zlib/
obj-$(CONFIG_THIRD_PARTY_MBEDTLS) 			+= mbedtls/
obj-$(CONFIG_THIRD_PARTY_OPENSSL) 			+= openssl/
obj-$(CONFIG_THIRD_PARTY_FREETYPE) 			+= freetype/
obj-$(CONFIG_THIRD_PARTY_LIBPNG) 			+= libpng/
obj-$(CONFIG_THIRD_PARTY_JPEG) 				+= jpeg/
obj-$(CONFIG_THIRD_PARTY_JPEGDEC) 			+= jpegdec/
obj-$(CONFIG_THIRD_PARTY_ICONV) 			+= iconv/
obj-$(CONFIG_THIRD_PARTY_TSLIB) 			+= tslib/
obj-$(CONFIG_THIRD_PARTY_QRCODE) 			+= qrcode/
obj-$(CONFIG_THIRD_PARTY_FFMPEG) 			+= ffmpeg/
obj-$(CONFIG_THIRD_PARTY_CURL) 				+= curl/
obj-$(CONFIG_THIRD_PARTY_JERRYSCRIPT) 		+= jerryscript/
obj-$(CONFIG_THIRD_PARTY_MININI)	 		+= minIni/
obj-$(CONFIG_THIRD_PARTY_AWTK) 				+= awtk/
obj-$(CONFIG_THIRD_PARTY_LVGL) 				+= lvgl/
obj-$(CONFIG_THIRD_PARTY_WSS) 				+= libwebsockets/


##build rule
.PHONY: all clean distclean

# ============================================================
# Build Process:
# ============================================================
# 1. 编译流程：
#    - 各子模块 Makefile 将中间产物写到 BUILD_THIRD_PARTY_DIR（out/build/third_party/...）
#    - 对 autotools 等需在源码树内改写的库，先在 $(BUILD_DIR)/_vendor/ 下做只读副本再配置/编译，不修改 third_party 源码目录
#    - 不要直接改各子目录里 $(库名)-版本号/ 下的上游源文件；定制放在子模块 Makefile，或对 $(BUILD_DIR)/_vendor 副本打补丁
#    - clean：删 BUILD_THIRD_PARTY_DIR、子模块 clean，并对各库 $(VENDOR_SRC) 执行 vendor_clean_intree
#
# 2. 编译中间产物（存放在 BUILD_DIR 中）：
#    - *.o 文件：目标文件，由 .c/.cpp 编译生成
#    - *.d 文件：依赖文件，记录源文件的依赖关系
#    - 示例路径：out/build/third_party/cjson/*.o
#
# 3. 编译后的库文件（存放在 BUILD_DIR 中）：
#    - *.a 文件：静态库文件，由多个 .o 文件打包而成
#    - *.so 文件：动态库文件（Linux），由多个 .o 文件链接而成
#    - 示例路径：out/build/third_party/cjson/libcjson.a
#                out/build/third_party/cjson/libcjson.so
#
# 4. Install 过程（从 BUILD_DIR 复制到 INSTALL_DIR）：
#    - 库文件：复制 *.a 和 *.so 到 $(THIRD_PARTY_INSTALL_DIR)/子模块名/lib/
#    - 头文件：复制 include/ 目录到 $(THIRD_PARTY_INSTALL_DIR)/子模块名/include/
#    - 示例路径：out/install/third_party/cjson/lib/libcjson.a
#                out/install/third_party/cjson/include/cjson.h
#
# 5. 跳过条件（重要）：
#    - 父 Makefile 只看 install/<module>/lib 是否存在；存在则 skipping，不看 build/。
#    - 因此删掉 out/.../install（或其中某个模块）会再次进入子模块 all。
#    - build/ 仍在时，子模块应复用已有产物再拷到 install（jpeg 已这样做）；
#      不应默认 configure+全量重编。
#
# ============================================================
all:
	@# 确保 BUILD_DIR 目录存在，然后遍历所有启用的子模块进行编译
	@if [ ! -d "$(BUILD_THIRD_PARTY_DIR)" ]; then \
		mkdir -p $(BUILD_THIRD_PARTY_DIR); \
	fi
	@for dir in $(obj-y); do \
		if [ -d "$(SRC_THIRD_PARTY_DIR)/$$dir" ]; then \
			echo "Building third_party module: $$dir"; \
			echo "  SRC: $(SRC_THIRD_PARTY_DIR)/$$dir"; \
			echo "  BUILD: $(BUILD_THIRD_PARTY_DIR)/$$dir (中间产物和库文件输出位置)"; \
			echo "  INSTALL: $(THIRD_PARTY_INSTALL_DIR)/$$dir (安装后的文件位置)"; \
    	if [ ! -d "$(THIRD_PARTY_INSTALL_DIR)/$$dir/lib" ]; then \
				$(MAKE) -C $(SRC_THIRD_PARTY_DIR)/$$dir \
					-f Makefile \
					-f ../vendor_rules.mk \
					BUILD_DIR=$(BUILD_THIRD_PARTY_DIR)/$$dir \
					THIRD_PARTY_INSTALL_DIR=$(THIRD_PARTY_INSTALL_DIR) \
					PROJECT_DIR=$(PROJECT_DIR) \
					COMPILATION_RULES=$(COMPILATION_RULES) \
					PROJECT_CONFIG_FILE=$(PROJECT_CONFIG_FILE); \
				if [ $$? -ne 0 ]; then exit 1; fi; \
			else \
				echo "Module $$dir already installed, skipping..."; \
			fi; \
    	fi; \
	done

clean:
	@echo "Cleaning third_party build files..."
	@echo "  Removing build directory: $(BUILD_THIRD_PARTY_DIR)"
	@echo "  (This removes all intermediate files: *.o, *.d, *.a, *.so)"
	@rm -rf $(BUILD_THIRD_PARTY_DIR)
	@# 子模块 clean，再清理误留在 $(VENDOR_SRC) 的 autotools 产物（vendor_rules.mk）
	@for dir in $(obj-y); do \
		if [ -d "$(SRC_THIRD_PARTY_DIR)/$$dir" ]; then \
			echo "  Cleaning module: $$dir"; \
			$(MAKE) -C $(SRC_THIRD_PARTY_DIR)/$$dir \
				-f Makefile \
				-f ../vendor_rules.mk \
				clean BUILD_DIR=$(BUILD_THIRD_PARTY_DIR)/$$dir 2>/dev/null || true; \
			$(MAKE) -C $(SRC_THIRD_PARTY_DIR)/$$dir \
				-f Makefile \
				-f ../vendor_rules.mk \
				vendor_clean_intree BUILD_DIR=$(BUILD_THIRD_PARTY_DIR)/$$dir 2>/dev/null || true; \
		fi; \
	done

distclean: clean



