# Copyright JS Foundation and other contributors, http://js.foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required (VERSION 2.8.12)

set(CMAKE_SYSTEM_NAME alios)
set(CMAKE_SYSTEM_PROCESSOR anyka)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "arm-none-eabi-g++")

SET(CMAKE_C_COMPILER_WORKS 1)
SET(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_C_FLAGS "-O0 -Wall -g2 -ggdb -mlittle-endian -mcpu=arm926ej-s -mfpu=auto -mfloat-abi=soft -mno-unaligned-access -fno-short-enums -fsigned-char -std=gnu11 -mlong-calls")
set(CMAKE_CXX_FLAGS "-O0 -Wall -mlittle-endian -mcpu=arm926ej-s -mfpu=auto -mfloat-abi=soft -mno-unaligned-access -fno-short-enums -fsigned-char -std=gnu11 -mlong-calls")

add_definitions(-DOS_NAME_ALIOS=1)

# Can be removed after gcc 5.2.0 support is removed (ref GCC_NOT_5_2_0)
set(CMAKE_EXE_LINKER_FLAGS "-nostdlib" CACHE STRING "Linker Base Flags")
