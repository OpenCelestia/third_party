cmake_minimum_required(VERSION 3.12.4)

SET(PROJECT_NAME lvgl)
project(${PROJECT_NAME})

# Version is determined from CONFIG_THIRD_PARTY_LVGL_VERSION in config.mk
# This CMakeLists.txt is used by the Makefile which sets VERSION from config.mk
# For now, we'll use a default that matches the actual source directory
# The Makefile will handle version-specific paths
SET(PROJECT_VERSION_MAJOR 9)
SET(PROJECT_VERSION_MINOR 3)
SET(PROJECT_VERSION_PATCH 0)
SET(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

SET(CMAKE_BUILD_TYPE "Release") # Release or Debug
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(CMAKE_VERBOSE_MAKEFILE 1)
endif()

SET(CONFIG_CHIP_SERIES $ENV{CONFIG_CHIP_SERIES})
SET(PLATFORM_PATH $ENV{PLATFORM_PATH})
SET(PORT_PATH $ENV{GUI_PORT_PATH}/lv-drivers)
SET(PROJECT_PATH $ENV{PROJECT_PATH})
# Do not assign CONFIG_OS_NAME only from $ENV — empty env breaks the linux branch (no .so).
# Order: non-empty env (FORCE) > existing cache (e.g. -DCONFIG_OS_NAME=linux) > default linux.
set(_lvgl_os "$ENV{CONFIG_OS_NAME}")
if(NOT _lvgl_os STREQUAL "")
    set(CONFIG_OS_NAME "${_lvgl_os}" CACHE STRING "OS for third_party lvgl" FORCE)
elseif("${CONFIG_OS_NAME}" STREQUAL "")
    set(CONFIG_OS_NAME "linux" CACHE STRING "OS for third_party lvgl" FORCE)
endif()
SET(INSTALL_PATH $ENV{THIRD_PARTY_INSTALL_DIR})
SET(HARDWARE_PATH $ENV{HARDWARE_PATH})
SET(CROSS_COMPILE $ENV{CROSS_COMPILE})

message(STATUS "lvgl: CONFIG_OS_NAME='${CONFIG_OS_NAME}' (env='$ENV{CONFIG_OS_NAME}')")

if (CONFIG_CHIP_SERIES STREQUAL "AK37D")
    add_definitions(-DCHIP_SERIES_AK37D=1)
elseif (CONFIG_CHIP_SERIES STREQUAL "AK37E")
    add_definitions(-DCHIP_SERIES_AK37E=1)
endif()

# Use the source directory with version (Makefile sets SRC_DIR to lvgl-$(VERSION))
# PROJECT_SOURCE_DIR is the CMakeLists.txt location (third_party/lvgl)
# Source files are in lvgl-${PROJECT_VERSION}/src/
file(GLOB_RECURSE SOURCES
     ${PROJECT_SOURCE_DIR}/lvgl-${PROJECT_VERSION}/src/*.c
     ${PROJECT_SOURCE_DIR}/lvgl-${PROJECT_VERSION}/src/*.cpp)

# Linux: same as cjson/zlib — ship both liblvgl.a and liblvgl.so from one compile (PIC).
# Other targets: static only (AliOS/RT-Thread toolchains typically omit usable DSO).
if (CONFIG_OS_NAME STREQUAL "linux")
    message(STATUS "lvgl: linux mode (build liblvgl.so + liblvgl.a)")
    add_library(lvgl_objs OBJECT ${SOURCES})
    set_target_properties(lvgl_objs PROPERTIES POSITION_INDEPENDENT_CODE ON)
    add_library(${PROJECT_NAME} SHARED $<TARGET_OBJECTS:lvgl_objs>)
    add_library(${PROJECT_NAME}_static STATIC $<TARGET_OBJECTS:lvgl_objs>)
    set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
else()
    message(STATUS "lvgl: non-linux mode (build liblvgl.a only)")
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
endif()

# NOTE: Toolchain selection should be done via -DCMAKE_<LANG>_COMPILER on the cmake command line
# (see third_party/lvgl/Makefile). Setting CMAKE_C_COMPILER here is too late for reliable effect.

# 根据不同平台，设置不同的参数
if (CONFIG_OS_NAME STREQUAL "alios")
    SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -mlittle-endian -mfpu=auto -mfloat-abi=soft -fno-delete-null-pointer-checks -mno-unaligned-access -fno-short-enums -fno-builtin-memcopy -fno-builtin-memmove -fno-builtin-memset -O2 -Wall -Wno-format-truncation -Wno-char-subscripts -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11 -nostdlib -static -mlong-calls")
    SET(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -mlittle-endian -mfpu=auto -mfloat-abi=soft -fno-delete-null-pointer-checks -mno-unaligned-access -fno-short-enums -fno-builtin-memcopy -fno-builtin-memmove -fno-builtin-memset -O2 -Wall -Wno-format-truncation -Wno-char-subscripts -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11 -nostdlib -static -mlong-calls")
    add_definitions(-DOS_NAME_ALIOS)
elseif (CONFIG_OS_NAME STREQUAL "rtt")
    SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O2 -ggdb -mcpu=arm926ej-s -std=c99 -mlittle-endian -fno-short-enums -fno-builtin -nostdlib -static -nostartfiles  -mlong-calls -DRT_USING_NEWLIB -DHAVE_CCONFIG_H")
    SET(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O2 -mcpu=arm926ej-s -std=c99 -mlittle-endian -fno-short-enums -fno-builtin -mfloat-abi=soft -mno-unaligned-access -nostdlib -static -nostartfiles -mlong-calls -DRT_USING_NEWLIB -DHAVE_CCONFIG_H")
    add_definitions(-DOS_NAME_RTT)
elseif (CONFIG_OS_NAME STREQUAL "linux")
    SET(CMAKE_C_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -std=gnu11")
    SET(CMAKE_C_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=gnu11")
    add_definitions(-DOS_NAME_LINUX)
endif()

# 根据不同平台，设置头文件路径
if (CONFIG_OS_NAME STREQUAL "alios")
    include_directories(${PLATFORM_PATH}/platform/include/alios)
    include_directories(${PLATFORM_PATH}/platform/include/alios/linux)
    include_directories(${PLATFORM_PATH}/platform/include/alios/posix)
    include_directories(${PLATFORM_PATH}/platform/include/alios/external)
    include_directories(${PLATFORM_PATH}/platform/include/alios/external/linux)
    include_directories(${PLATFORM_PATH}/platform/include/alios/internal)
    include_directories(${PLATFORM_PATH}/os/driver/include)
    include_directories(${PLATFORM_PATH}/os/kernel/kernel/include)
    include_directories(${PLATFORM_PATH}/os/kernel/kernel/include/posix)

elseif (CONFIG_OS_NAME STREQUAL "rtt")
    include_directories(${PLATFORM_PATH}/os/driver/include)
    include_directories(${PLATFORM_PATH}/os/kernel_rtt/bsp/anyka)
    include_directories(${PLATFORM_PATH}/os/kernel_rtt/bsp/anyka/include)
    include_directories(${PLATFORM_PATH}/os/kernel_rtt/bsp/anyka/include/linux)
    include_directories(${PLATFORM_PATH}/os/kernel_rtt/bsp/anyka/osal)
    include_directories(${PLATFORM_PATH}/os/kernel_rtt/bsp/anyka/include/sys)
    include_directories(${PLATFORM_PATH}/os/kernel_rtt/components/drivers/include)
    include_directories(${PLATFORM_PATH}/os/kernel_rtt/components/dfs/include)
    include_directories(${PLATFORM_PATH}/os/kernel_rtt/components/libc/pthreads)
    include_directories(${PLATFORM_PATH}/os/kernel_rtt/components/finsh)
    include_directories(${PLATFORM_PATH}/os/kernel_rtt/components/newlib)
    include_directories(${PLATFORM_PATH}/os/kernel_rtt/include)
    include_directories(${PLATFORM_PATH}/os/kernel_rtt/include/libc)
elseif (CONFIG_OS_NAME STREQUAL "linux")
    include_directories(${INSTALL_PATH}/tslib/include)
endif()

# Include directories - use version-specific paths
include_directories(${PROJECT_SOURCE_DIR}/lvgl-${PROJECT_VERSION})
include_directories(${PROJECT_SOURCE_DIR}/lvgl-${PROJECT_VERSION}/src/libs/freetype)
include_directories(${PROJECT_SOURCE_DIR}/lvgl-${PROJECT_VERSION}/src/draw/sw)
include_directories(${PROJECT_SOURCE_DIR}/lvgl-${PROJECT_VERSION}/src/display)

include_directories(${PORT_PATH})
include_directories(${PLATFORM_PATH}/platform/include)

# Prefer HARDWARE_PATH; build.sh exports HARDWARE_DIR.
if (HARDWARE_PATH STREQUAL "" AND NOT "$ENV{HARDWARE_DIR}" STREQUAL "")
    SET(HARDWARE_PATH $ENV{HARDWARE_DIR})
endif()
if (NOT HARDWARE_PATH STREQUAL "")
    include_directories(${HARDWARE_PATH}/include)
endif()

# include path of freetype
include_directories(${INSTALL_PATH}/freetype/include/freetype2)

# lv_conf.h lives in project/<product>/include (or out/.../build/project/include).
# Fall back to PROJECT_DIR / PROJECT_GEN_INCLUDE_DIR when PROJECT_PATH was not exported.
if (PROJECT_PATH STREQUAL "" AND NOT "$ENV{PROJECT_DIR}" STREQUAL "")
    SET(PROJECT_PATH $ENV{PROJECT_DIR})
endif()
if (NOT PROJECT_PATH STREQUAL "")
    include_directories(${PROJECT_PATH}/include)
endif()
if (NOT "$ENV{PROJECT_GEN_INCLUDE_DIR}" STREQUAL "")
    include_directories($ENV{PROJECT_GEN_INCLUDE_DIR})
endif()

set(_lv_conf_h "")
if (NOT PROJECT_PATH STREQUAL "" AND EXISTS "${PROJECT_PATH}/include/lv_conf.h")
    set(_lv_conf_h "${PROJECT_PATH}/include/lv_conf.h")
elseif (EXISTS "$ENV{PROJECT_GEN_INCLUDE_DIR}/lv_conf.h")
    set(_lv_conf_h "$ENV{PROJECT_GEN_INCLUDE_DIR}/lv_conf.h")
elseif (EXISTS "${PROJECT_SOURCE_DIR}/lv_conf.h")
    set(_lv_conf_h "${PROJECT_SOURCE_DIR}/lv_conf.h")
endif()
if (_lv_conf_h STREQUAL "")
    message(FATAL_ERROR
        "lvgl: lv_conf.h not found. Set PROJECT_PATH/PROJECT_DIR to project/<product> "
        "(expected .../include/lv_conf.h), or place lv_conf.h next to this CMakeLists.txt.")
endif()
message(STATUS "lvgl: using lv_conf.h at ${_lv_conf_h}")
# Make sure the preprocessor picks this file (do not rely on ../../lv_conf.h alone).
add_compile_definitions(LV_CONF_INCLUDE_SIMPLE)
get_filename_component(_lv_conf_dir "${_lv_conf_h}" DIRECTORY)
include_directories(${_lv_conf_dir})

# Shared liblvgl.so must resolve FreeType / libpng at link time.
# Otherwise exec fails: "Error relocating liblvgl.so: FT_*/png_*: symbol not found".
# Embed .a when .so is missing (Nova libpng is static-only today).
if (CONFIG_OS_NAME STREQUAL "linux")
    set(_ft_so "${INSTALL_PATH}/freetype/lib/libfreetype.so")
    set(_ft_a  "${INSTALL_PATH}/freetype/lib/libfreetype.a")
    if (EXISTS "${_ft_so}")
        target_link_libraries(${PROJECT_NAME} PRIVATE "${_ft_so}")
        message(STATUS "lvgl: link shared ${_ft_so}")
    elseif (EXISTS "${_ft_a}")
        target_link_libraries(${PROJECT_NAME} PRIVATE "${_ft_a}")
        message(WARNING "lvgl: libfreetype.so missing, embedding ${_ft_a} into liblvgl.so")
    else()
        message(WARNING "lvgl: freetype not found under ${INSTALL_PATH}/freetype/lib")
    endif()
    # Always close png_* if present: stale objs / LV_USE_LIBPNG / port confusion.
    set(_png_so "${INSTALL_PATH}/libpng/lib/libpng.so")
    set(_png_a  "${INSTALL_PATH}/libpng/lib/libpng.a")
    set(_z_so   "${INSTALL_PATH}/zlib/lib/libz.so")
    set(_z_a    "${INSTALL_PATH}/zlib/lib/libz.a")
    if (EXISTS "${_png_so}")
        target_link_libraries(${PROJECT_NAME} PRIVATE "${_png_so}")
        message(STATUS "lvgl: link shared ${_png_so}")
    elseif (EXISTS "${_png_a}")
        target_link_libraries(${PROJECT_NAME} PRIVATE "${_png_a}")
        message(STATUS "lvgl: embed ${_png_a} into liblvgl.so")
    endif()
    if (EXISTS "${_z_so}")
        target_link_libraries(${PROJECT_NAME} PRIVATE "${_z_so}")
    elseif (EXISTS "${_z_a}")
        target_link_libraries(${PROJECT_NAME} PRIVATE "${_z_a}")
    endif()
endif()
