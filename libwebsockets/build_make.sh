#!/bin/bash

# $1: source dir, $2: CROSS_COMPILE prefix, $3: install prefix
# $4: absolute CC path (optional, falls back to $2gcc resolved via PATH)
# $5: absolute CXX path (optional, falls back to $2g++ resolved via PATH)
CROSS_CC="${4:-${2}gcc}"
CROSS_CXX="${5:-${2}g++}"

# Use mbedtls instead of OpenSSL to avoid HMAC_CTX incomplete type issues
# This matches the commented configuration in new version
cmake -S $1 -DCMAKE_C_COMPILER=$CROSS_CC -DCMAKE_CXX_COMPILER=$CROSS_CXX -DCMAKE_INSTALL_PREFIX=$3 \
  -DLWS_WITH_SSL=ON \
  -DLWS_WITH_MBEDTLS=ON \
  -DLWS_WITH_ZLIB=ON \
  -DLWS_MBEDTLS_INCLUDE_DIRS=$3/mbedtls/include \
  -DMBEDTLS_LIBRARY=$3/mbedtls/lib/libmbedtls.a \
  -DMBEDX509_LIBRARY=$3/mbedtls/lib/libmbedx509.a \
  -DMBEDCRYPTO_LIBRARY=$3/mbedtls/lib/libmbedcrypto.a \
  -DZLIB_INCLUDE_DIR=$3/zlib/include \
  -DZLIB_LIBRARY=$3/zlib/lib/libz.a \
  -DLWS_WITHOUT_TESTAPPS=ON \
  -DLWS_WITH_SHARED=ON \
  -DLWS_WITH_STATIC=ON \
  -DLWS_IPV6=ON \
  -DLWS_WITH_LIBUV=OFF \
  -DLWS_WITH_HTTP2=OFF
