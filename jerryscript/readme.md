# 1、单独模块编译

在模块根目录下执行以下指
COMPILATION_RULES 为平台的rules.mk文件
CONFIG_OS_NAME为linux、rtt、aos选一个
THIRD_PARTY_INSTALL_PATH参数为lib生成后存放的目录

编译的时候需要export以上三个值再make
或make的时候带入以上两个参数


# 2、新版本移植说明
 (1)、增加cmake/toolchain-anyka
 (2)、增加jerry-port/anyka


# 3、模块接口调用说明

生成lib和include，可以加载lib及调用对应include中的接口。