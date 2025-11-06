#!/bin/sh -xe

CC="${CC:-clang}"

CFLAGS="-std=c99 -Wall -Wextra -pedantic -fshort-enums -Wvla -Wdouble-promotion -Wno-implicit-fallthrough"

WASM_CFLAGS="-O3 --target=wasm32 -flto -nostdlib -fvisibility=hidden"
WASM_CFLAGS+=" -mbulk-memory -msimd128"
WASM_LDFLAGS="--no-entry --strip-all --lto-O3 --allow-undefined --export-dynamic"

WASM_FLAGS="$WASM_CFLAGS"
for FLAG in $WASM_LDFLAGS
do
  WASM_FLAGS+=" -Wl,$FLAG"
done

mkdir -p wasm

SRCS="$(ls src/*.c)"

for SRC in $(ls src/wasm/*.c)
do
  OUTPUT="wasm/$(basename ${SRC%.*}).wasm"
  $CC $CFLAGS $WASM_FLAGS -o $OUTPUT $SRC $SRCS
done
