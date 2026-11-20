import os
import platform
import shutil
from awtk_config_common import TKC_STATIC_LIBS
from awtk_config_common import joinPath, toWholeArchive, genIdlAndDefEx, setEnvSpawn,genDllLinkFlags,copySharedLib,cleanSharedLib

OS_NAME = platform.system()

def joinPath(root, subdir):
  return os.path.normpath(os.path.join(root, subdir))

PROJECT_PATH = os.environ.get('PROJECT_PATH')
PLATFORM_PATH = os.environ.get('PLATFORM_PATH')
INSTALL_PATH = os.environ.get('THIRD_PARTY_INSTALL_PATH')
FRAMEWORKS_PATH = os.environ.get('FRAMEWORKS_PATH')
GUI_PORT_PATH = os.environ.get('GUI_PORT_PATH')
COMPILATION_RULES = os.environ.get('COMPILATION_RULES')
CONFIG_OS_NAME = os.environ.get('CONFIG_OS_NAME')
CONFIG_CHIP_SERIES = os.environ.get('CONFIG_CHIP_SERIES')
HARDWARE_PATH = os.environ.get('HARDWARE_PATH')
AWTK_ROOT = os.environ.get('AWTK_ROOT')
CROSS_COMPILE = os.environ.get('CROSS_COMPILE')

SDK_LIB_DIR=[os.path.abspath(PLATFORM_PATH + '/platform/lib'), os.path.abspath(PLATFORM_PATH + '/platform/ext_lib'), os.path.abspath(PLATFORM_PATH + '/os')]
SDK_KERNEL_ROOT=joinPath(PLATFORM_PATH, 'os/kernel')
SDK_OSDRV_ROOT=joinPath(PLATFORM_PATH, 'os/driver')

PATH_INCLUDE = '-I' + PLATFORM_PATH + '/platform/include '
PATH_INCLUDE = PATH_INCLUDE + '-I' + PLATFORM_PATH + '/platform/include/alios '
PATH_INCLUDE = PATH_INCLUDE + '-I' + PLATFORM_PATH + '/platform/include/alios/aos '
PATH_INCLUDE = PATH_INCLUDE + '-I' + PLATFORM_PATH + '/platform/include/alios/linux '
PATH_INCLUDE = PATH_INCLUDE + '-I' + PLATFORM_PATH + '/platform/include/alios/posix '
PATH_INCLUDE = PATH_INCLUDE + '-I' + PLATFORM_PATH + '/platform/include/alios/external '
PATH_INCLUDE = PATH_INCLUDE + '-I' + PLATFORM_PATH + '/platform/include/alios/external/aos '
PATH_INCLUDE = PATH_INCLUDE + '-I' + PLATFORM_PATH + '/platform/include/alios/external/linux '
PATH_INCLUDE = PATH_INCLUDE + '-I' + PLATFORM_PATH + '/platform/include/alios/external/posix '
PATH_INCLUDE = PATH_INCLUDE + '-I' + PLATFORM_PATH + '/platform/include/alios/internal '
PATH_INCLUDE = PATH_INCLUDE + '-I' + SDK_KERNEL_ROOT + '/components/drivers/include '
PATH_INCLUDE = PATH_INCLUDE + '-I' + SDK_KERNEL_ROOT + '/kernel/include '
PATH_INCLUDE = PATH_INCLUDE + '-I' + SDK_KERNEL_ROOT + '/kernel/include/posix '
PATH_INCLUDE = PATH_INCLUDE + '-I' + SDK_OSDRV_ROOT + '/include '

PATH_INCLUDE = PATH_INCLUDE + '-I' + PROJECT_PATH + '/include '

PATH_INCLUDE = PATH_INCLUDE + '-I' + HARDWARE_PATH + '/include '
PATH_INCLUDE = PATH_INCLUDE + '-I' + INSTALL_PATH + '/iconv/include '

if CONFIG_OS_NAME == 'linux' :
    TKC_STATIC_LIBS = ['debugger', 'fscript_ext', 'streams', 'conf_io', 'hal', 'xml', 'charset',
                       'csv', 'ubjson', 'compressors', 'miniz', 'tkc_core', 'mbedtls']

    TSLIB_INC_DIR = INSTALL_PATH + '/tslib/include'
    PATH_INCLUDE = PATH_INCLUDE + '-I' + TSLIB_INC_DIR + ' '

    TSLIB_LIB_DIR = os.path.abspath(INSTALL_PATH + '/tslib/lib');
else:
    TKC_STATIC_LIBS = ['fscript_ext', 'conf_io', 'xml', 'csv', 'ubjson', 'compressors', 'miniz', 'tkc_core']
    TSLIB_INC_DIR=''
    TSLIB_LIB_DIR=''

def lcd_devices_is_egl(lcd_devices):
  if lcd_devices =='egl_for_fsl' or lcd_devices =='egl_for_x11' or lcd_devices =='egl_for_gbm' :
    return True
  return False

CWD=os.path.normpath(os.path.abspath(os.path.dirname(__file__)));

TK_LINUX_FB_ROOT = joinPath(GUI_PORT_PATH, 'awtk-linux-fb')
TK_ROOT          = joinPath(CWD, AWTK_ROOT)
TK_SRC           = joinPath(TK_ROOT, 'src')
TK_3RD_ROOT      = joinPath(TK_ROOT, '3rd')
GTEST_ROOT       = joinPath(TK_ROOT, '3rd/gtest/googletest')

BUILD_DIR        = joinPath(TK_ROOT, 'build')
BIN_DIR          = joinPath(BUILD_DIR, 'bin')
LIB_DIR          = joinPath(BUILD_DIR, 'lib')
VAR_DIR          = joinPath(BUILD_DIR, 'var')
TK_DEMO_ROOT     = joinPath(TK_ROOT, 'demos')

LCD_DIR        = joinPath(TK_LINUX_FB_ROOT, 'awtk-port/lcd_linux')
INPUT_DIR      = joinPath(TK_LINUX_FB_ROOT, 'awtk-port/input_thread')

PATH_INCLUDE = PATH_INCLUDE + '-I' + TK_ROOT + '/src '
PATH_INCLUDE = PATH_INCLUDE + '-I' + TK_LINUX_FB_ROOT + '/awtk-port '
PATH_INCLUDE = PATH_INCLUDE + '-I' + TK_LINUX_FB_ROOT + '/awtk-port/anyka '

# lcd devices
LCD_DEVICES='fb'
# LCD_DEVICES='drm'
# LCD_DEVICES='egl_for_fsl'
# LCD_DEVICES='egl_for_x11'
# LCD_DEVICES='egl_for_gbm'

NANOVG_BACKEND=''
VGCANVAS='NANOVG'
#VGCANVAS='CAIRO'
#VGCANVAS='NANOVG_PLUS'
if LCD_DEVICES =='fb' or LCD_DEVICES =='drm' :
  LCD='LINUX_FB'
  NANOVG_BACKEND='AGGE'
elif lcd_devices_is_egl(LCD_DEVICES) :
  LCD='FB_GL'
  NANOVG_BACKEND='GLES2'

#INPUT_ENGINE='null'
#INPUT_ENGINE='spinyin'
#INPUT_ENGINE='t9'
#INPUT_ENGINE='t9ext'
INPUT_ENGINE='pinyin'

COMMON_CCFLAGS=' -DHAS_STD_MALLOC -DHAS_STDIO -DHAS_FAST_MEMCPY -DWITH_VGCANVAS -DWITH_UNICODE_BREAK '

if CONFIG_OS_NAME == 'linux' :
    COMMON_CCFLAGS=COMMON_CCFLAGS+' -DLOAD_ASSET_WITH_MMAP=1 -DWITH_SOCKET=1 '

COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITH_ASSET_LOADER -DWITH_FS_RES -DHAS_GET_TIME_US64=1 ' 
COMMON_CCFLAGS=COMMON_CCFLAGS+' -DSTBTT_STATIC -DSTB_IMAGE_STATIC -DWITH_STB_IMAGE -DWITH_STB_FONT -DWITH_TEXT_BIDI=1 '
COMMON_CCFLAGS=COMMON_CCFLAGS+' -DAPP_TYPE=APP_MOBILE -DLINUX_FB '

COMMON_CCFLAGS=COMMON_CCFLAGS+' -DHAS_AWTK_CONFIG -DUSE_GUI_MAIN -DMINIZ_NO_TIME -DWITH_NANOVG_SOFT -DWITH_ASSET_LOADER_ZIP '


if LCD_DEVICES =='fb' :
  COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITH_NANOVG_AGGE -DWITH_LINUX_FB -DWITH_FAST_LCD_PORTRAIT '
elif LCD_DEVICES =='drm' :
  COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITH_NANOVG_AGGE -DWITH_LINUX_DRM -DWITH_FAST_LCD_PORTRAIT '
elif lcd_devices_is_egl(LCD_DEVICES) :
  COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITH_GPU_GL -DWITH_GPU_GLES2 -DWITH_GPU -DWITH_LINUX_EGL '


if INPUT_ENGINE == 't9':
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_T9 '
elif INPUT_ENGINE == 't9ext' :
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_T9EXT'
elif INPUT_ENGINE == 'pinyin' :
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_PINYIN '
elif INPUT_ENGINE == 'spinyin' :
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_SPINYIN '
elif INPUT_ENGINE == 'null' :
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_NULL '

GRAPHIC_BUFFER='default'
#GRAPHIC_BUFFER='jzgpu'
#if GRAPHIC_BUFFER == 'jzgpu':
#  COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITH_JZGPU'

#only for c compiler flags
COMMON_CFLAGS=''
#COMMON_CFLAGS=COMMON_CFLAGS+' -std=gnu99 '

OS_LIBS=[]
OS_LIBPATH=[SDK_LIB_DIR]
OS_CPPPATH=[]
OS_LINKFLAGS=''
OS_SUBSYSTEM_CONSOLE=''
OS_SUBSYSTEM_WINDOWS=''

OS_FLAGS = ''
OS_FLAGS = OS_FLAGS + PATH_INCLUDE

if CONFIG_OS_NAME =='rtt' :
    COMMON_CCFLAGS = COMMON_CCFLAGS+' -DCLOCK_MONOTONIC=1 '
    OS_FLAGS = OS_FLAGS + '-DOS_NAME_RTT=1 '
    OS_FLAGS = OS_FLAGS + '-O2 -mcpu=arm926ej-s -std=c99 -mlittle-endian -fno-short-enums -fno-builtin -nostdlib -static -nostartfiles -mlong-calls '
    OS_FLAGS = OS_FLAGS + '-DRT_USING_NEWLIB -D_POSIX_REALTIME_SIGNALS -DHAVE_CCONFIG_H '

elif CONFIG_OS_NAME =='alios' :
    OS_FLAGS = OS_FLAGS + '-DOS_NAME_ALIOS=1 '
    OS_FLAGS = OS_FLAGS + '-mlittle-endian -mcpu=arm926ej-s -mfpu=auto -mfloat-abi=soft -fno-delete-null-pointer-checks -mno-unaligned-access \
               -fno-short-enums -fno-builtin-memcopy -fno-builtin-memmove -fno-builtin-memset -O2 -Wall \
               -Wno-char-subscripts -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11 -nostdlib -static -nostartfiles -mlong-calls '

elif CONFIG_OS_NAME =='linux' :
    OS_FLAGS = OS_FLAGS + '-DOS_NAME_LINUX=1 '
    OS_FLAGS = OS_FLAGS + '-Wall -Os -fno-strict-aliasing -std=gnu99 '

TOOLS_PREFIX = CROSS_COMPILE

if CONFIG_CHIP_SERIES == 'AK37E' :
    OS_FLAGS = OS_FLAGS + '-DCHIP_TYPE_AK37E -DWITH_ANYKA_VDEC -DWITH_G2D=1 -DWITH_ANYKA_G2D=1 '
elif CONFIG_CHIP_SERIES == 'AK37D' :
    OS_FLAGS = OS_FLAGS + '-DCHIP_TYPE_AK37D -DWITH_ANYKA_VDEC -DWITH_G2D=1 -DWITH_ANYKA_G2D=1 '

"""

TSLIB_LIB_DIR=''
TSLIB_INC_DIR=''

#for build tslib
#TSLIB_INC_DIR=joinPath(TK_LINUX_FB_ROOT, '3rd/tslib/src')
#TSLIB_LIB_DIR=joinPath(TK_LINUX_FB_ROOT, '3rd/tslib/src/.libs')

#for prebuild tslib
#TSLIB_LIB_DIR='/opt/tslib/lib'
#TSLIB_INC_DIR='/opt/tslib/include'

#for qemu
TOOLS_PREFIX='/opt/qemu/buildroot-2021.02.2/output/host/bin/arm-linux-'
"""

#for poky cortex-a9
#TOOLS_PREFIX='/opt/poky/1.7/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-'
#OS_FLAGS=' -Wall -mfloat-abi=hard -fno-strict-aliasing '

#for v3s mango
#TOOLS_PREFIX='/opt/v3s/mango/tools/external-toolchain/bin/arm-linux-gnueabi-'
#OS_FLAGS='-std=gnu99 -mthumb -mabi=aapcs-linux -mlittle-endian -fdata-sections -ffunction-sections -mcpu=cortex-a7 -mtune=cortex-a7 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp '

#for pc build
#TOOLS_PREFIX=''

#for android
#TOOLS_PREFIX='/opt/android-ndk-r20b/toolchains/llvm/prebuilt/linux-x86_64/bin/'
#TOOLS_PREFIX='/Users/jim/android/android-ndk-r21d/toolchains/llvm/prebuilt/darwin-x86_64/bin/'
TARGET_ARCH = platform.architecture();

CC=TOOLS_PREFIX+'gcc',
CXX=TOOLS_PREFIX+'g++',
LD=TOOLS_PREFIX+'g++',
AR=TOOLS_PREFIX+'ar',
RANLIB=TOOLS_PREFIX+'ranlib',
STRIP=TOOLS_PREFIX+'strip',

if CONFIG_OS_NAME == 'linux' :
    OS_LIBS = ['stdc++', 'pthread', 'rt', 'm', 'dl']

OS_LINKFLAGS= OS_LINKFLAGS + ' -Wl,-rpath=./bin -Wl,-rpath=./ ' 

if LCD_DEVICES =='drm' :
  #for drm
  #OS_CPPPATH += ['/usr/include/libdrm']
  OS_LIBS = ['drm'] + OS_LIBS
elif LCD_DEVICES =='egl_for_fsl':
  #for egl for fsl
  OS_FLAGS=OS_FLAGS + ' -DEGL_API_FB '
  OS_LIBS = [ 'GLESv2', 'EGL'] + OS_LIBS
elif LCD_DEVICES =='egl_for_x11' :
  #for egl for fsl
  OS_LIBS = [ 'X11', 'EGL', 'GLESv2' ] + OS_LIBS
elif LCD_DEVICES =='egl_for_gbm' :
  #for egl for gbm
  #OS_CPPPATH += ['/usr/include/libdrm', '/usr/include/GLES2']
  OS_LIBS = [ 'drm', 'gbm', 'EGL', 'GLESv2' ] + OS_LIBS

if CONFIG_OS_NAME == 'linux' :
  COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DLINUX -DHAS_PTHREAD -fPIC '

COMMON_CCFLAGS = COMMON_CCFLAGS+' -DWITH_DATA_READER_WRITER=1 '
COMMON_CCFLAGS = COMMON_CCFLAGS+' -DWITH_EVENT_RECORDER_PLAYER=1 '
COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_WIDGET_TYPE_CHECK=1 '

COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DHAS_PTHREAD -DENABLE_CURSOR '
if TSLIB_LIB_DIR != '':
  COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DHAS_TSLIB '

OS_PROJECTS=[]
CFLAGS=COMMON_CFLAGS
LINKFLAGS=OS_LINKFLAGS;
LIBPATH=[LIB_DIR, BIN_DIR] + OS_LIBPATH
CCFLAGS=OS_FLAGS + COMMON_CCFLAGS 

STATIC_LIBS =['awtk_global', 'fscript_ext_widgets', 'extwidgets', 'widgets', 'awtk_linux_fb', 'base', 'gpinyin', 'linebreak', 'fribidi']
STATIC_LIBS += TKC_STATIC_LIBS
if TSLIB_LIB_DIR != '':
  SHARED_LIBS=['awtk'] + OS_LIBS + ['ts'];
else:
  SHARED_LIBS=['awtk'] + OS_LIBS;

if VGCANVAS == 'NANOVG':
  TK_ROOT_VAR = joinPath(VAR_DIR, 'awtk')
  OS_PROJECTS = [ joinPath(TK_ROOT_VAR, '3rd/nanovg/SConscript') ]
  OS_CPPPATH += [joinPath(TK_3RD_ROOT, 'nanovg'),  joinPath(TK_3RD_ROOT, 'nanovg/gl'),  joinPath(TK_3RD_ROOT, 'nanovg/base') ]
  if LCD_DEVICES =='fb' or LCD_DEVICES =='drm' :
    STATIC_LIBS = STATIC_LIBS + ['nanovg-agge', 'agge', 'nanovg']  + OS_LIBS
    AWTK_DLL_DEPS_LIBS = ['nanovg-agge', 'agge', 'nanovg'] + OS_LIBS
  elif lcd_devices_is_egl(LCD_DEVICES) :
    CCFLAGS += ' -DWITH_NANOVG_GLES2 -DWITH_NANOVG_GL -DWITH_NANOVG_GPU '
    STATIC_LIBS = STATIC_LIBS + ['glad', 'nanovg']  + OS_LIBS
    AWTK_DLL_DEPS_LIBS = ['glad', 'nanovg'] + OS_LIBS
elif VGCANVAS == 'CAIRO':
  TK_ROOT_VAR = joinPath(VAR_DIR, 'awtk')
  OS_PROJECTS = [joinPath(TK_ROOT_VAR, '3rd/cairo/SConscript'), joinPath(TK_ROOT_VAR, '3rd/pixman/SConscript')]
  OS_CPPPATH += [joinPath(TK_3RD_ROOT, 'cairo'),  joinPath(TK_3RD_ROOT, 'pixman') ]
  STATIC_LIBS = STATIC_LIBS + ['cairo', 'pixman']  + OS_LIBS
  AWTK_DLL_DEPS_LIBS= ['cairo', 'pixman'] + OS_LIBS
  CCFLAGS += ' -DWITH_VGCANVAS_CAIRO -DHAVE_CONFIG_H -DCAIRO_WIN32_STATIC_BUILD '

elif VGCANVAS == 'NANOVG_PLUS':
  TK_ROOT_VAR = joinPath(VAR_DIR, 'awtk')
  OS_PROJECTS = [ joinPath(TK_ROOT_VAR, '3rd/nanovg_plus/SConscript') ]
  OS_CPPPATH += [joinPath(TK_3RD_ROOT, 'nanovg_plus/gl'), joinPath(TK_3RD_ROOT, 'nanovg_plus/base') ]
  if lcd_devices_is_egl(LCD_DEVICES) :
    CCFLAGS += ' -DWITH_NANOVG_PLUS_GPU -DWITH_NANOVG_GPU -DWITH_GPU_GL -DWITH_GPU_GLES2 -DNVGP_GLES2 '
    STATIC_LIBS = STATIC_LIBS + ['glad', 'nanovg_plus']  + OS_LIBS
    AWTK_DLL_DEPS_LIBS = ['glad', 'nanovg_plus'] + OS_LIBS


LIBS=STATIC_LIBS
AWTK_STATIC_LIBS = LIBS
OS_WHOLE_ARCHIVE =toWholeArchive(LIBS)

CPPPATH=[TK_ROOT, 
  TK_SRC, 
  TK_3RD_ROOT, 
  LCD_DIR, 
  INPUT_DIR, 
  joinPath(TK_SRC, 'ext_widgets'), 
  joinPath(TK_SRC, 'custom_widgets'), 
  joinPath(TK_ROOT, 'tools'), 
  joinPath(TK_3RD_ROOT, 'agge'), 
  joinPath(TK_3RD_ROOT, 'agg/include'), 
  joinPath(TK_3RD_ROOT, 'mbedtls/include'), 
  joinPath(TK_3RD_ROOT, 'mbedtls/3rdparty/everest/include'), 
  joinPath(TK_3RD_ROOT, 'fribidi'), 
  joinPath(TK_3RD_ROOT, 'libunibreak'), 
  joinPath(TK_3RD_ROOT, 'gpinyin/include'), 
  joinPath(TK_3RD_ROOT, 'gtest/googletest'), 
  joinPath(TK_3RD_ROOT, 'gtest/googletest/include'), 
  ] + OS_CPPPATH

if TSLIB_LIB_DIR != '':
  LIBS = LIBS + ['ts']
  LIBPATH = [TSLIB_LIB_DIR] + LIBPATH;
  CPPPATH = [TSLIB_INC_DIR] + CPPPATH;

os.environ['LCD'] = LCD
os.environ['LCD_DEVICES'] = LCD_DEVICES
os.environ['TARGET_ARCH'] = 'arm'
os.environ['BIN_DIR'] = BIN_DIR;
os.environ['LIB_DIR'] = LIB_DIR;
os.environ['TK_ROOT'] = TK_ROOT;
os.environ['CCFLAGS'] = CCFLAGS;
os.environ['VGCANVAS'] = VGCANVAS
os.environ['INPUT_ENGINE'] = INPUT_ENGINE;
os.environ['TSLIB_LIB_DIR'] = TSLIB_LIB_DIR;
os.environ['NANOVG_BACKEND'] = NANOVG_BACKEND;
os.environ['TK_3RD_ROOT'] = TK_3RD_ROOT;
os.environ['GTEST_ROOT'] = GTEST_ROOT;
os.environ['TOOLS_NAME'] = '';
os.environ['GRAPHIC_BUFFER'] = GRAPHIC_BUFFER;

if CONFIG_OS_NAME == 'linux' :
  os.environ['WITH_AWTK_SO'] = 'true'

if LCD_DEVICES =='fb' or LCD_DEVICES =='drm' :
  os.environ['NATIVE_WINDOW'] = 'raw';
elif lcd_devices_is_egl(LCD_DEVICES) :
  os.environ['NATIVE_WINDOW'] = 'fb_gl';


os.environ['OS_WHOLE_ARCHIVE'] = OS_WHOLE_ARCHIVE;
os.environ['AWTK_DLL_DEPS_LIBS'] = ';'.join(AWTK_DLL_DEPS_LIBS)
os.environ['STATIC_LIBS'] = ';'.join(STATIC_LIBS)
os.environ['CROSS_COMPILE'] = str(not TOOLS_PREFIX == '')

def has_custom_cc():
    return True

def copySharedLib(src, dst, name):
  src = os.path.join(src, 'bin/lib'+name+'.so')
  src = os.path.normpath(src);
  dst = os.path.normpath(dst);

  if os.path.dirname(src) == dst:
      return

  if not os.path.exists(src):
    print('Can\'t find ' + src + '. Please build '+name+'before!')
  else:
    if not os.path.exists(dst):
        os.makedirs(dst)
    shutil.copy(src, dst)
    print(src + '==>' + dst);
